#include <moveit_simple_controller_manager/follow_joint_trajectory_controller_handle.h>
#include <control_msgs/FollowJointTrajectoryAction.h>
#include <actionlib/server/simple_action_server.h>
#include <ros/ros.h>
#include <hardware_interface/joint_state_interface.h>
#include <joint_state_controller/joint_state_controller.h>
//#include <control_msgs/FollowJointTrajectoryResult.h>
//#include <trajectory_msgs/JointTrajectoryPoint.h>

class MyRobotActionServer
{
public:

  MyRobotActionServer(const std::string &name);
  ~MyRobotActionServer();

private:
  hardware_interface::JointStateInterface js_iface;
  hardware_interface::JointStateHandle jh1, jh2,jh3;
  joint_state_controller::JointStateController jsc;
  double cur_positions[3], cur_velocities[3], cur_efforts[3];
  
  ros::NodeHandle nh_;
  ros::NodeHandle private_node_handle;
  actionlib::SimpleActionServer<control_msgs::FollowJointTrajectoryAction> as_;
  ros::Timer state_pub_timer;
  
  void executeCallback(const control_msgs::FollowJointTrajectoryGoalConstPtr & goal);
  void pubCallback(const ros::TimerEvent&);
};

MyRobotActionServer::MyRobotActionServer(const std::string &name)
: as_(nh_, name, boost::bind(&MyRobotActionServer::executeCallback, this,
    _1), false),
  private_node_handle("~"),
  jh1("joint_1", &cur_positions[0], &cur_positions[0], &cur_positions[0]),
  jh2("joint_2", &cur_positions[1], &cur_velocities[1], &cur_efforts[1]),
  jh3("joint_3", &cur_positions[2], &cur_velocities[3], &cur_efforts[2])
{

  //initial state
  for(int i=0;i<3;i++ ) {
    cur_positions[i] = 0;
    cur_velocities[i] = 0;
    cur_efforts[i] = 0;
  }

  as_.start();
  js_iface.registerHandle(jh1);
  js_iface.registerHandle(jh2);
  js_iface.registerHandle(jh3);
  jsc.init(&js_iface, nh_, private_node_handle);
  jsc.starting(ros::Time::now());
  
  state_pub_timer = nh_.createTimer(ros::Duration(0.1), 
      boost::bind(&MyRobotActionServer::pubCallback, this, _1), false, true);  
  
}

MyRobotActionServer::~MyRobotActionServer() {
  jsc.stopping(ros::Time::now());
}

void MyRobotActionServer::pubCallback(const ros::TimerEvent&) {
  //ROS_INFO_NAMED("MyRobotActionServer", "jsc.update");
  jsc.update(ros::Time::now(), ros::Duration());
}

void MyRobotActionServer::executeCallback(const control_msgs::FollowJointTrajectoryGoalConstPtr & goal)
{
  ros::Time startTime = ros::Time::now();
  for (std::vector<trajectory_msgs::JointTrajectoryPoint>::const_iterator
       via = goal->trajectory.points.begin(), end = goal->trajectory.points.end();
       via != end; ++via)
  {
    if (as_.isPreemptRequested()) {
      as_.setPreempted();
      return;
    }  
    for(int i=0;i<3;i++ ) {
      cur_positions[i] = via->positions[i];
      cur_velocities[i] = via->velocities[i];
      //cur_efforts[i] = via->effort[i];
    }

    ros::Duration waitTime = via->time_from_start - (ros::Time::now() - startTime);
    if (waitTime.toSec() > std::numeric_limits<float>::epsilon())
    {
      ROS_INFO("Fake execution: waiting %0.1fs for next via point, %ld remaining", waitTime.toSec(), end - via);
      waitTime.sleep();
    }
  }
  /*const trajectory_msgs::JointTrajectoryPoint &first = goal->trajectory.points.front();
  const trajectory_msgs::JointTrajectoryPoint &last = goal->trajectory.points.back();
  ROS_INFO_STREAM("first effort size: " << first.effort.size());  
  ROS_INFO_STREAM("last effort size: " << last.effort.size());
  ROS_INFO_STREAM("first velocities size: " << first.velocities.size());  
  ROS_INFO_STREAM("last velocities size: " << last.velocities.size());  
*/
  ROS_INFO("went to target position");  
  as_.setSucceeded(control_msgs::FollowJointTrajectoryResult(), "MyRobotActionServer has succeeded");
}

int main(int argc, char ** argv)
{
  ros::init(argc, argv, "myrobot_action_server");

  MyRobotActionServer server("myrobot_controller/follow_joint_trajectory");
  ROS_INFO_NAMED("MyRobotActionServer", "MyRobot controller is running...");
  ros::spin();

  return 0;
}
