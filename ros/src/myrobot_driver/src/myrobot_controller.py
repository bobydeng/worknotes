#!/usr/bin/env python
#
# Software License Agreement (BSD License)
#
# Copyright (c) 2012-2014, Southwest Research Institute
# Copyright (c) 2014-2015, TU Delft Robotics Institute
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Southwest Research Institute, nor the names
#    of its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

import rospy
import copy
import threading
import serial
import time
import re

import sys
if sys.version_info.major == 2:
    import Queue
else:
    import queue as Queue

# Publish
from sensor_msgs.msg import JointState
from control_msgs.msg import FollowJointTrajectoryFeedback
from industrial_msgs.msg import RobotStatus

# Subscribe
from trajectory_msgs.msg import JointTrajectory
from trajectory_msgs.msg import JointTrajectoryPoint

# Services
from industrial_msgs.srv import GetRobotInfo, GetRobotInfoResponse

# Reference
from industrial_msgs.msg import TriState, RobotMode, ServiceReturnCode, DeviceInfo


RX_BUFFER_SIZE = 128
BAUD_RATE = 115200
ENABLE_STATUS_REPORTS = True
REPORT_INTERVAL = 1.0 # seconds

STATUS_IDLE = 0
STATUS_RUN = 1
STATUS_OTHER = 100
STATUS_NO_CONNECTION = -1

DEG_PER_RAD = 180/3.1416

#<Idle|MPos:0.000,0.000,0.000|FS:0,0>
#<Idle|MPos:0.000,0.000,0.000|FS:0,0|WCO:0.000,0.000,0.000>
#<Run|MPos:0.000,0.000,0.000|FS:52,0>
status_pattern = r'<(\S+)\|MPos:(\S+),(\S+),(\S+)\|FS:(\d+\.*\d*),(\d+\.*\d*).*'
gcode_format = 'G1 X{:-.3f} Y{:-.3f} Z{:-.3f} F{:-.3f}'


"""
MyrobotController

This class is the motion controller for an industrial robot.

This class IS threadsafe

"""
class MyrobotController():
    """
    Constructor of motion controller
    """
    def __init__(self, num_joints, initial_joint_state, update_rate = 100, buffer_size = 0):
        # Class lock
        self.lock = threading.Lock()

        # Motion loop update rate (higher update rates result in smoother simulated motion)
        self.update_rate = update_rate
        rospy.logdebug("Setting motion update rate (hz): %f", self.update_rate)

        # Initialize joint position
        self.joint_positions = initial_joint_state
        rospy.logdebug("Setting initial joint state: %s", str(initial_joint_state))

        # Initialize motion buffer (contains joint position lists)
        self.motion_buffer = Queue.Queue(buffer_size)
        rospy.logdebug("Setting motion buffer size: %i", buffer_size)

        # Shutdown signal
        self.sig_shutdown = False

        # Stop signal
        self.sig_stop = False

        # Initialize
        com_device_file = rospy.get_param('com_device_file', 'dev/ttyACM0')        
        self.s = serial.Serial(com_device_file,BAUD_RATE)
        # Wake up grbl
        print "Initializing Grbl..."
        self.s.write("\r\n\r\n")
        
        self.g_count = 0
        self.l_count = 0
        self.c_line = []
        self.status = STATUS_IDLE
        self.last_report_time = rospy.Time.now()
        
        self.comReceiverThread = threading.Thread(target=self.com_receiver)
        self.comReceiverThread.daemon = True
        self.comReceiverThread.start()        
        
        time.sleep(1)        
        self.s.write("G93\n")
        self.s.write("$G\n")
        
        # Start status report periodic timer
        if ENABLE_STATUS_REPORTS :
            self.timerThread = threading.Thread(target=self._status_timer)
            self.timerThread.daemon = True
            self.timerThread.start()

        # Motion thread
        self.motion_thread = threading.Thread(target=self._motion_worker)
        self.motion_thread.daemon = True
        self.motion_thread.start()
        
    def __del__(self):
      self.s.close()

    def _send_status_query(self):
        self.s.write('?')
        
    def _status_timer(self) :
        while not self.sig_shutdown:
          self._send_status_query()
          elapse = rospy.Time.now() - self.last_report_time
          if elapse > rospy.Duration(5):
            self.status = STATUS_NO_CONNECTION
          time.sleep(REPORT_INTERVAL)

    def prepare_trajector(self):
      wait_cycle = 0
      with self.lock:
        total_len = len(self.c_line)
        
      while total_len > 0 and (wait_cycle < 50 or self.status == STATUS_RUN):
        time.sleep(0.1)
        with self.lock:
          total_len = len(self.c_line)
        wait_cycle +=1
        
      #buffer status reset
      with self.lock:
        self.g_count = 0
        self.l_count = 0
        self.c_line = []

    """
    """
    def add_motion_waypoint(self, point):
        self.motion_buffer.put(point)


    """
    """
    def get_joint_positions(self):
        with self.lock:
            return self.joint_positions[:]

    """
    """
    def is_in_motion(self):
        return not self.motion_buffer.empty()

    def status_code(self):
      return  self.status

    """
    """
    def shutdown(self):
        self.sig_shutdown = True
        rospy.logdebug('Motion_Controller shutdown signaled')

    """
    """
    def stop(self):
        rospy.logdebug('Motion_Controller stop signaled')
        with self.lock:
            self._clear_buffer()
            self.sig_stop = True

    """
    """
    def interpolate(self, last_pt, current_pt, alpha):
        intermediate_pt = JointTrajectoryPoint()
        for last_joint, current_joint in zip(last_pt.positions, current_pt.positions):
            intermediate_pt.positions.append(last_joint + alpha*(current_joint-last_joint))
        intermediate_pt.time_from_start = last_pt.time_from_start + rospy.Duration(alpha*(current_pt.time_from_start.to_sec() - last_pt.time_from_start.to_sec()))
        return intermediate_pt

    """
    """
    def accelerate(self, last_pt, current_pt, current_time, delta_time):
        intermediate_pt = JointTrajectoryPoint()
        for last_joint, current_joint, last_vel, current_vel in zip(last_pt.positions, current_pt.positions, last_pt.velocities, current_pt.velocities):
            delta_x = current_joint-last_joint
            dv = current_vel+last_vel
            a1 = 6*delta_x/pow(delta_time,2) - 2*(dv+last_vel)/delta_time
            a2 = -12*delta_x/pow(delta_time,3) + 6*dv/pow(delta_time,2)
            current_pos = last_joint + last_vel*current_time + a1*pow(current_time,2)/2 +a2*pow(current_time,3)/6
            intermediate_pt.positions.append(current_pos)
        intermediate_pt.time_from_start = last_pt.time_from_start + rospy.Duration(current_time)
        return intermediate_pt

    """
    """
    def _clear_buffer(self):
        with self.motion_buffer.mutex:
            self.motion_buffer.queue.clear()


    """
    """
    def _move_to(self, point, dur):
        with self.lock:
          total_len = sum(self.c_line)
        while not self.sig_shutdown and not self.sig_stop and (total_len >= RX_BUFFER_SIZE-5 | self.s.inWaiting()):
          time.sleep(0.1)
          with self.lock:
            total_len = sum(self.c_line)
    
        if self.sig_shutdown:
          return
        
        if self.sig_stop:
          rospy.logdebug('Stopping motion immediately, clearing stop signal')
          with self.lock:
            self.sig_stop = False
          return

        #rospy.sleep(dur)
        pos = point.positions
        dur_sec = dur.to_sec()
        if dur_sec < 0.0001:
          rospy.loginfo("duration to short to run, dur: " + str(dur_sec))
          return
          
        self.l_count += 1 # Iterate line counter
        l_block = gcode_format.format(pos[1]*DEG_PER_RAD, pos[2]*DEG_PER_RAD, pos[0]*1000, 60/dur_sec)
        self.c_line.append(len(l_block)+1)
        self.s.write(l_block + '\n') # Send g-code block to grbl
        rospy.loginfo("SND>"+str(self.l_count)+": \"" + l_block + "\"")

        #with self.lock:
        #    self.joint_positions = point.positions[:]
            #rospy.loginfo('Moved to position: %s in %s', str(self.joint_positions), str(dur))


    def com_receiver(self):
      while not self.sig_shutdown:
          grbl_out = self.s.readline().strip() # Wait for grbl response
          #rospy.loginfo("  MSG: \""+grbl_out+"\"")
          if grbl_out.find('ok') < 0 and grbl_out.find('error') < 0 :
              rospy.loginfo("  MSG: \""+grbl_out+"\"") # Debug response
              if grbl_out.startswith('<') :
                self.extract_status(grbl_out)
          else :
              with self.lock:
                if len(self.c_line) > 0:
                  self.g_count += 1 # Iterate g-code counter
                  rospy.loginfo("  REC<"+str(self.g_count)+": \""+grbl_out+"\"")
                  del self.c_line[0] # Delete the block character count corresponding to the last 'ok'

    def extract_status(self, grbl_out):
        match = re.search(status_pattern, grbl_out)
        if match is not None:
          [status, mx, my, mz, f0, f1] = match.groups()
          with self.lock:
            self.joint_positions = [float(mz)/1000, float(mx)/DEG_PER_RAD, float(my)/DEG_PER_RAD]
            self.last_report_time = rospy.Time.now()
            if status == 'Idle':
              self.status = STATUS_IDLE
            elif status == 'Run':
              self.status = STATUS_RUN
            else: 
              self.status = STATUS_OTHER

    """
    """
    def _motion_worker(self):
        rospy.logdebug('Starting motion worker in myrobot controller')
        move_duration = rospy.Duration()
        #if self.update_rate != 0.:
        #    update_duration = rospy.Duration(1./self.update_rate)
        last_goal_point = JointTrajectoryPoint()

        with self.lock:
            last_goal_point.positions = self.joint_positions[:]

        while not self.sig_shutdown:
            try:
                current_goal_point = self.motion_buffer.get()

                # If the current time from start is less than the last, then it's a new trajectory
                if current_goal_point.time_from_start < last_goal_point.time_from_start:
                    move_duration = current_goal_point.time_from_start
                # Else it's an existing trajectory and subtract the two
                else:
                    # If current move duration is greater than update_duration, move arm to interpolated joint position
                    # Provide an exception to this rule: if update rate is <=0, do not add interpolated points
                    move_duration = current_goal_point.time_from_start - last_goal_point.time_from_start
                    '''if self.update_rate > 0:
                        starting_goal_point = copy.deepcopy(last_goal_point)
                        full_duration = move_duration.to_sec()
                        while update_duration < move_duration and not sig_stop:
                            if not starting_goal_point.velocities or not current_goal_point.velocities:
                                intermediate_goal_point = self.interpolate(last_goal_point, current_goal_point, update_duration.to_sec()/move_duration.to_sec())
                            else:
                                intermediate_goal_point = self.accelerate(starting_goal_point, current_goal_point, full_duration-move_duration.to_sec()+update_duration.to_sec(), full_duration)
                            self._move_to(intermediate_goal_point, update_duration.to_sec()) #TODO should this use min(update_duration, 0.5*move_duration) to smooth timing?
                            last_goal_point = copy.deepcopy(intermediate_goal_point)
                            move_duration = current_goal_point.time_from_start - intermediate_goal_point.time_from_start
                '''
                self._move_to(current_goal_point, move_duration)
                last_goal_point = copy.deepcopy(current_goal_point)

            except Exception as e:
                rospy.logerr('Unexpected exception: %s', e)

        rospy.logdebug("Shutting down motion controller")




"""
MyrobotController

The controller adheres to the ROS-Industrial robot driver specification:

http://www.ros.org/wiki/Industrial/Industrial_Robot_Driver_Spec

TODO: Currently the controller only supports the bare minimum motion
interface.

TODO: Interfaces to add:
Joint streaming
All services

"""
class MyrobotControllerNode():
    """
    Constructor of myobot controller
    """
    def __init__(self):
        rospy.init_node('myrobot_controller')

        # Class lock
        self.lock = threading.Lock()

        # Publish rate (hz)
        self.pub_rate = rospy.get_param('pub_rate', 10.0)
        rospy.logdebug("Setting publish rate (hz) based on parameter: %f", self.pub_rate)

        # Joint names
        def_joint_names = ["joint_1", "joint_2", "joint_3", "joint_4", "joint_5", "joint_6"]
        self.joint_names = rospy.get_param('controller_joint_names', def_joint_names)
        if len(self.joint_names) == 0:
            rospy.logwarn("Joint list is empty, did you set the 'controller_joint_names' parameter?")
        rospy.loginfo("manipulator has %d joints: %s", len(self.joint_names), ", ".join(self.joint_names))

        # Setup initial joint positions
        num_joints = len(self.joint_names)
        initial_joint_state = rospy.get_param('initial_joint_state', [0]*num_joints)
        same_len = len(initial_joint_state) == num_joints
        all_num  = all(type(x) is int or type(x) is float for x in initial_joint_state)
        if not same_len or not all_num:
            initial_joint_state = [0]*num_joints
            rospy.logwarn("Invalid initial_joint_state parameter, defaulting to all-zeros "
                "(len: %s, types: %s).", same_len, all_num)
        rospy.loginfo("Using initial joint state: %s", str(initial_joint_state))

        # retrieve update rate
        motion_update_rate = rospy.get_param('motion_update_rate', 100.);  #set param to 0 to ignore interpolated motion
        self.motion_ctrl = MyrobotController(num_joints, initial_joint_state, update_rate=motion_update_rate)

        # Published to joint states
        rospy.logdebug("Creating joint state publisher")
        self.joint_state_pub = rospy.Publisher('joint_states', JointState, queue_size=1)

        # Published to joint feedback
        rospy.logdebug("Creating joint feedback publisher")
        self.joint_feedback_pub = rospy.Publisher('feedback_states', FollowJointTrajectoryFeedback, queue_size=1)

        rospy.logdebug("Creating robot status publisher")
        self.robot_status_pub = rospy.Publisher('robot_status', RobotStatus, queue_size=1)

        # Subscribe to a joint trajectory
        rospy.logdebug("Creating joint trajectory subscriber")
        self.joint_path_sub = rospy.Subscriber('joint_path_command', JointTrajectory, self.trajectory_callback)

        # JointStates timed task (started automatically)
        period = rospy.Duration(1.0/self.pub_rate)
        rospy.logdebug('Setting up publish worker with period (sec): %s', str(period.to_sec()))
        rospy.Timer(period, self.publish_worker)

        # GetRobotInfo service server and pre-cooked svc response
        self.get_robot_info_response = self._init_robot_info_response()
        self.svc_get_robot_info = rospy.Service('get_robot_info', GetRobotInfo, self.cb_svc_get_robot_info)

        # Clean up init
        rospy.on_shutdown(self.motion_ctrl.shutdown)


    """
    Service callback for GetRobotInfo() service. Returns fake information.
    """
    def cb_svc_get_robot_info(self, req):
        # return cached response instance
        return self.get_robot_info_response


    """
    The publish worker is executed at a fixed rate.  This publishes the various
    state and status information for the robot.
    """
    def publish_worker(self, event):
        self.joint_state_publisher()
        self.robot_status_publisher()

    """
    The joint state publisher publishes the current joint state and the current
    feedback state (as these are closely related)
    """
    def joint_state_publisher(self):
        try:
            joint_state_msg = JointState()
            joint_fb_msg = FollowJointTrajectoryFeedback()
            time = rospy.Time.now()

            with self.lock:
                #Joint states
                joint_state_msg.header.stamp = time
                joint_state_msg.name = self.joint_names
                joint_state_msg.position = self.motion_ctrl.get_joint_positions()

                self.joint_state_pub.publish(joint_state_msg)

                #Joint feedback
                joint_fb_msg.header.stamp = time
                joint_fb_msg.joint_names = self.joint_names
                joint_fb_msg.actual.positions = self.motion_ctrl.get_joint_positions()

                self.joint_feedback_pub.publish(joint_fb_msg)

        except Exception as e:
            rospy.logerr('Unexpected exception in joint state publisher: %s', e)


    """
    The robot status publisher publishes the current simulated robot status.

    The following values are hard coded:
     - robot always in AUTO mode
     - drives always powered
     - motion always possible
     - robot never E-stopped
     - no error code
     - robot never in error

    The value of 'in_motion' is derived from the state of the
    MotionControllerSimulator.
    """
    def robot_status_publisher(self):
        try:
            msg = RobotStatus()
            msg.mode.val            = RobotMode.AUTO
            msg.e_stopped.val       = TriState.FALSE
            msg.drives_powered.val  = TriState.TRUE
            msg.motion_possible.val = TriState.TRUE
            msg.in_motion.val       = self.motion_ctrl.is_in_motion()
            msg.error_code = self.motion_ctrl.status_code()
            if msg.error_code < 0:
              msg.in_error.val        = TriState.TRUE
            else:
              msg.in_error.val        = TriState.FALSE
            self.robot_status_pub.publish(msg)

        except Exception as e:
            rospy.logerr('Unexpected exception: %s', e)


    """
    Trajectory subscription callback (gets called whenever a joint trajectory
    is received).

    @param msg_in: joint trajectory message
    @type  msg_in: JointTrajectory
    """
    def trajectory_callback(self, msg_in):
        try:
            rospy.logdebug('Received trajectory with %s points, executing callback', str(len(msg_in.points)))

            if self.motion_ctrl.is_in_motion():
                if len(msg_in.points) > 0:
                    rospy.logerr('Received trajectory while still in motion, trajectory splicing not supported')
                else:
                    rospy.logdebug('Received empty trajectory while still in motion, stopping current trajectory')
                self.motion_ctrl.stop()

            else:
                self.motion_ctrl.prepare_trajector()
                for point in msg_in.points:
                    point = self._to_controller_order(msg_in.joint_names, point)
                    self.motion_ctrl.add_motion_waypoint(point)

        except Exception as e:
            rospy.logerr('Unexpected exception: %s', e)

        rospy.logdebug('Exiting trajectory callback')


    """
    Remaps point to controller joint order

    @param keys:   keys defining joint value order
    @type  keys:   list
    @param point:  joint trajectory point
    @type  point:  JointTrajectoryPoint

    @return point: reorder point
    @type point: JointTrajectoryPoint
    """
    def _to_controller_order(self, keys, point):
        #rospy.loginfo('to controller order, keys: %s, point: %s', str(keys), str(point))
        pt_rtn = copy.deepcopy(point)
        pt_rtn.positions = self._remap_order(self.joint_names, keys, point.positions)
        if point.velocities:
            pt_rtn.velocities = self._remap_order(self.joint_names, keys, point.velocities)

        return pt_rtn

    def _remap_order(self, ordered_keys, value_keys, values):
        #rospy.loginfo('remap order, ordered_keys: %s, value_keys: %s, values: %s', str(ordered_keys), str(value_keys), str(values))
        ordered_values = []

        ordered_values = [0]*len(ordered_keys)
        mapping = dict(zip(value_keys, values))
        #rospy.loginfo('maping: %s', str(mapping))

        for i in range(len(ordered_keys)):
            ordered_values[i] = mapping[ordered_keys[i]]
            pass

        return ordered_values

    """
    Constructs a GetRobotInfoResponse instance with either default data,
    or data provided by the user.
    """
    def _init_robot_info_response(self):
        if not rospy.has_param('~robot_info'):
            # if user did not provide data, we generate some
            import rospkg
            rp = rospkg.RosPack()
            irs_version = rp.get_manifest('myrobot_driver').version
            robot_info = dict(
                controller=dict(
                    model='Myrobot Controller',
                    serial_number='0123456789',
                    sw_version=irs_version),
                robots=[
                    dict(
                        model='Myrobot Controller Manipulator',
                        serial_number='9876543210',
                        sw_version=irs_version)
                ])
        else:
            # otherwise use only the data user has provided (and nothing more)
            robot_info = rospy.get_param('~robot_info')

        resp = GetRobotInfoResponse()
        resp.controller = DeviceInfo(**robot_info['controller'])

        # add info on controlled robot / motion group
        if len(robot_info['robots']) > 0:
            robot = robot_info['robots'][0]
            resp.robots.append(DeviceInfo(**robot))

        if len(robot_info['robots']) > 1:
            # simulator simulates a single robot / motion group
            rospy.logwarn("Multiple robots / motion groups defined in "
                "'robot_info' parameter, ignoring all but first element")

        # always successfull
        resp.code.val = ServiceReturnCode.SUCCESS
        return resp


if __name__ == '__main__':
    try:
        rospy.loginfo('Starting myrobot_controller')
        controller = MyrobotControllerNode()
        rospy.spin()
    except Exception as e:
        rospy.logerr('Unexpected exception: %s', e)
        
