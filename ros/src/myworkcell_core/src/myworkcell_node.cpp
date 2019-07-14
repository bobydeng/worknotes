#include <math.h>       /* sqrt */
#include <ros/ros.h>
#include <myworkcell_core/LocalizePart.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_msgs/GetPositionIK.h>

// ROS Trajectory Action server definition
#include <control_msgs/FollowJointTrajectoryAction.h>

// Means by which we communicate with above action-server
#include <actionlib/client/simple_action_client.h>

// Includes the descartes robot model we will be using
#include <descartes_moveit/ikfast_moveit_state_adapter.h>

// Includes the descartes trajectory type we will be using
#include <descartes_trajectory/axial_symmetric_pt.h>
#include <descartes_trajectory/cart_trajectory_pt.h>

// Includes the planner we will be using
#include <descartes_planner/dense_planner.h>

// Includes the utility function for converting to trajectory_msgs::JointTrajectory's
#include <descartes_utilities/ros_conversions.h>

class ScanNPlan
{
public:
  ScanNPlan();
  int initDescartModel();
  void start(const std::string& base_frame);

private:
  const std::string PLANNING_GROUP = "manipulator";
  ros::NodeHandle nh;
  ros::ServiceClient vision_client_;
  ros::ServiceClient ik_client_;
  actionlib::SimpleActionClient<control_msgs::FollowJointTrajectoryAction> ac;
  descartes_core::RobotModelPtr descartes_model;
  descartes_planner::DensePlanner descartes_planner;
  
  std::vector<double> getCurrentJointState(const std::string& topic);
  void setConstraints(moveit::planning_interface::MoveGroupInterface move_group);
  bool compute_ik(moveit::planning_interface::MoveGroupInterface move_group, 
    geometry_msgs::PoseStamped &pose_stamped, sensor_msgs::JointState &joint_state);
  std::vector<descartes_core::TrajectoryPtPtr> makePath(
    const Eigen::Vector3d &startPoint, const Eigen::Vector3d &endPoint);
  descartes_core::TrajectoryPtPtr makeCartesianPoint(const Eigen::Affine3d& pose, double dt);
  bool executeTrajectory(const trajectory_msgs::JointTrajectory& trajectory);
  int moveAlongLine(const Eigen::Vector3d &startPoint, const Eigen::Vector3d &endPoint);
};

ScanNPlan::ScanNPlan():
  ac("joint_trajectory_action", true),
  descartes_model(new descartes_moveit::IkFastMoveitStateAdapter())
{
  vision_client_ = nh.serviceClient<myworkcell_core::LocalizePart>("localize_part");
  ik_client_ = nh.serviceClient<moveit_msgs::GetPositionIK>("compute_ik");
}
  
int ScanNPlan::initDescartModel() {
  // Create a Follow Joint Trajectory Action Client
  if (!ac.waitForServer(ros::Duration(2.0)))
  {
    ROS_ERROR("Could not connect to action server");
    return -1;
  }

  const std::string robot_description = "robot_description";

  // Name of frame in which you are expressing poses. Typically "world_frame" or "base_link".
  const std::string world_frame = "base_link";

  // tool center point frame (name of link associated with tool). The robot's flange is typically "tool0" but yours
  // could be anything. We typically have our tool's positive Z-axis point outward from the grinder, welder, etc.
  const std::string tcp_frame = "tool0";

  // Before you can use a model, you must call initialize. This will load robot models and sanity check the model.
  if (!descartes_model->initialize(robot_description, PLANNING_GROUP, world_frame, tcp_frame))
  {
    ROS_INFO("Could not initialize robot model");
    return -1;
  }
  
  descartes_model->setCheckCollisions(true); // Let's turn on collision checking.

  // Like the model, you also need to call initialize on the planner
  if (!descartes_planner.initialize(descartes_model))
  {
    ROS_ERROR("Failed to initialize planner");
    return -2;
  }
}
  

void ScanNPlan::start(const std::string& base_frame)
{
  ROS_INFO("Attempting to localize part");
  myworkcell_core::LocalizePart loc_srv;
  loc_srv.request.base_frame = base_frame;
  ROS_INFO_STREAM("Requesting pose in base frame: " << base_frame);
  if(!vision_client_.call(loc_srv))
  {
    ROS_ERROR("Could not localize part");
    return;
  }

  ROS_INFO_STREAM("part localized: \n" << loc_srv.response);

  moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);
  //move_group.setPoseReferenceFrame(base_frame);
  //setConstraints(move_group);

  //for(int i=-6;i<7;i++) {
  //geometry_msgs::Pose pose;
  //pose.position.z =0.06;// + i*0.005;// 0.08;
  //pose.position.x =0.1;
  //pose.position.y =0.03;//0.01*i;
     
  geometry_msgs::Pose &pose = loc_srv.response.pose;
  pose.position.y -=0.055;

  ROS_INFO("Setting target position...");
  //method 1: not work for DOF3
  //move_group.setPoseTarget(pose);
  
  //method 2.1:
  move_group.setJointValueTarget(pose);
  
  //method 2.2:
  //geometry_msgs::PoseStamped pose_stamped;
  //pose_stamped.header.frame_id = std::string("/")+ base_frame;
  //pose_stamped.header.stamp = ros::Time::now();
  //pose_stamped.pose.position = pose.position;
  //move_group.setJointValueTarget(pose_stamped);
  
  //method 2.3:
  //sensor_msgs::JointState joint_state
  //if(!compute_ik(move_group, pose, joint_state)) {
  //  return;
  //}
  //move_group.setJointValueTarget(joint_state.position);
 
  //method 4:
  //move_group.setNamedTarget("home_left");
  
  ROS_INFO("Starting planning...");
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  bool success = (move_group.plan(plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
  ROS_INFO("Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
  if(success) {
    move_group.move();
  }
  
  //} //end for
  geometry_msgs::Point position = move_group.getCurrentPose().pose.position;
  Eigen::Vector3d startPoint(position.x, position.y, position.z - 0.05);
  Eigen::Vector3d endPoint = startPoint + Eigen::Vector3d(0,0.11, 0);
  
  ROS_INFO_STREAM("start point:\n" << startPoint);
  ROS_INFO_STREAM("end point:\n" << endPoint);
  
  moveAlongLine(startPoint, endPoint);
}
  
std::vector<double> ScanNPlan::getCurrentJointState(const std::string& topic)
{
  sensor_msgs::JointStateConstPtr state = ros::topic::waitForMessage<sensor_msgs::JointState>(topic, ros::Duration(0.0));
  if (!state) throw std::runtime_error("Joint state message capture failed");
  ROS_INFO_STREAM("Current joint state: \n" << *state);
  return state->position;
}

void ScanNPlan::setConstraints(moveit::planning_interface::MoveGroupInterface move_group) {
  moveit_msgs::Constraints constraints = move_group.getPathConstraints();
  ROS_INFO_STREAM("path constraints: \n" << constraints);    
  //moveit_msgs::Constraints constraints;
  moveit_msgs::OrientationConstraint oc;
  oc.link_name = "tool0";
  oc.header.frame_id = "/world";
  oc.weight = 1.0;
  //oc.orientation.x = 0.0;
  //oc.orientation.y = 0.0;
  //oc.orientation.z = 0;
  oc.orientation.w = 1;
  oc.absolute_x_axis_tolerance = 3.14;
  oc.absolute_y_axis_tolerance = 3.14;
  oc.absolute_z_axis_tolerance = 3.14;

  constraints.orientation_constraints.push_back(oc);
  move_group.setPathConstraints(constraints);
  
  ROS_INFO_STREAM("path constraints: \n" << constraints);  
}

bool ScanNPlan::compute_ik(moveit::planning_interface::MoveGroupInterface move_group, 
  geometry_msgs::PoseStamped &pose_stamped, sensor_msgs::JointState &joint_state) {
  
  ROS_INFO("Getting current_state...");
  robot_state::RobotStatePtr current_state = move_group.getCurrentState();

  ROS_INFO_STREAM("current pose: \n" << move_group.getCurrentPose());

  moveit_msgs::GetPositionIK::Request request;
  request.ik_request.group_name = move_group.getName();

  const robot_state::JointModelGroup* joint_model_group =
    current_state->getJointModelGroup(move_group.getName());
  current_state->copyJointGroupPositions(joint_model_group,
    request.ik_request.robot_state.joint_state.position);    
  
  request.ik_request.avoid_collisions = true;

  //pose_stamped.pose.orientation.x = 0.0;
  //pose_stamped.pose.orientation.y = 0.0;
  //pose_stamped.pose.orientation.z = 0;
  //pose_stamped.pose.orientation.w = 1;
  ROS_INFO_STREAM("target pose: \n" << pose_stamped);

  request.ik_request.pose_stamped = pose_stamped;
  moveit_msgs::GetPositionIK::Response response;
  
  ROS_INFO("Calculating IK...");
  ik_client_.call(request, response);
  ROS_INFO_STREAM(
      "Result: " << ((response.error_code.val == response.error_code.SUCCESS) ? "True " : "False ")
                 << response.error_code.val);

  if(response.error_code.val != response.error_code.SUCCESS)
  {
    ROS_ERROR("Could not find ik solution");
    return false;
  }

  ROS_INFO_STREAM("IK solution: \n" << response.solution);
  joint_state = response.solution.joint_state;
  return true;
}
    
int ScanNPlan::moveAlongLine(const Eigen::Vector3d& startPoint, const Eigen::Vector3d& endPoint) {
  // 2. The next thing to do is to generate a path for the robot to follow. The description of this path is one of the
  // cool things about Descartes. The source of this path is where this library ties into your application: it could
  // come from CAD or from surfaces that were "scanned".

  // Make the path by calling a helper function. See makePath()'s definition for more discussion about paths.
  std::vector<descartes_core::TrajectoryPtPtr> points = makePath(startPoint, endPoint);

  // 4. Now, for the planning itself. This typically happens in two steps. First, call planPath(). This function takes
  // your input trajectory and expands it into a large kinematic "graph". Failures at this point indicate that the
  // input path may not have solutions at a given point (because of reach/collision) or has two points with no way
  // to connect them.
  if (!descartes_planner.planPath(points))
  {
    ROS_ERROR("Could not solve for a valid path");
    return -3;
  }

  // After expanding the graph, we now call 'getPath()' which searches the graph for a minimum cost path and returns
  // the result. Failures here (assuming planPath was good) indicate that your path has solutions at every waypoint
  // but constraints prevent a solution through the whole path. Usually this means a singularity is hanging out in the
  // middle of your path: the robot can solve all the points but not in the same arm configuration.
  std::vector<descartes_core::TrajectoryPtPtr> result;
  if (!descartes_planner.getPath(result))
  {
    ROS_ERROR("Could not retrieve path");
    return -4;
  }

  // 5. Translate the result into something that you can execute. In ROS land, this means that we turn the result into
  // a trajectory_msgs::JointTrajectory that's executed through a control_msgs::FollowJointTrajectoryAction. If you
  // have your own execution interface, you can get joint values out of the results in the same way.

  // get joint names - this could be from the robot model, or from the parameter server.
  std::vector<std::string> names;
  nh.getParam("controller_joint_names", names);

  // Create a JointTrajectory
  trajectory_msgs::JointTrajectory joint_solution;
  joint_solution.joint_names = names;

  // Define a default velocity. Descartes points without specified timing will use this value to limit the
  // fastest moving joint. This usually effects the first point in your path the most.
  const static double default_joint_vel = 0.5; // rad/s
  if (!descartes_utilities::toRosJointPoints(*descartes_model, result, default_joint_vel, joint_solution.points))
  {
    ROS_ERROR("Unable to convert Descartes trajectory to joint points");
    return -5;
  }

  // 6. Send the ROS trajectory to the robot for execution
  if (!executeTrajectory(joint_solution))
  {
    ROS_ERROR("Could not execute trajectory!");
    return -6;
  }

  // Wait till user kills the process (Control-C)
  ROS_INFO("Done!");
  return 0;
}
  
descartes_core::TrajectoryPtPtr ScanNPlan::makeCartesianPoint(const Eigen::Affine3d& pose, double dt)
{
  using namespace descartes_core;
  using namespace descartes_trajectory;

  return TrajectoryPtPtr( new CartTrajectoryPt( TolerancedFrame(pose), TimingConstraint(dt)) );
}

descartes_core::TrajectoryPtPtr makeTolerancedCartesianPoint(const Eigen::Affine3d& pose, double dt)
{
  using namespace descartes_core;
  using namespace descartes_trajectory;
  return TrajectoryPtPtr( new AxialSymmetricPt(pose, M_PI, AxialSymmetricPt::Z_AXIS, TimingConstraint(dt)) );
}

std::vector<descartes_core::TrajectoryPtPtr> ScanNPlan::makePath(
  const Eigen::Vector3d& startPoint, const Eigen::Vector3d& endPoint)
{
  // In Descartes, trajectories are composed of "points". Each point describes what joint positions of the robot can
  // satisfy it. You can have a "joint point" for which only a single solution is acceptable. You might have a
  // fully defined cartesian point for which many (8 or 16) different robot configurations might work. You could
  // allow extra tolerances in any of these and even more points satisfy the constraints.

  // In this first tutorial, we're just going to describe a simple cartesian trajectory that moves the robot
  // along a line in the XY plane.

  // Step 1: Let's start by just doing the math to generate the poses we want.

  // First thing, let's generate a pattern with its origin at zero. We'll define another transform later that
  // can move it to somewere more convenient.
  
  Eigen::Vector3d diff = endPoint - startPoint;
  
  ROS_INFO_STREAM("difference:\n" << diff);

  double len = sqrt(diff.dot(diff));
  
  ROS_INFO("Line length: %f", len);  
  const static double step_size = 0.005;
  //const static double step_size = 0.0005;
  int num_steps = ceil(len/step_size);
  ROS_INFO("steps: %d", num_steps);    
  
  Eigen::Vector3d delta =diff/num_steps;
  //double time_between_points = 0.1;
  double time_between_points = step_size/0.002 * 0.1;
  ROS_INFO("time_between_points: %f", time_between_points);

  EigenSTL::vector_Affine3d pattern_poses;
for(int m = 0 ; m < 3; ++m) {
  for (int i = 1; i <= num_steps; ++i)
  {
    // Create a pose and initialize it to identity
    Eigen::Affine3d pose = Eigen::Affine3d::Identity();
    // set the translation (we're moving along a line in Y)
    pose.translation() = startPoint + delta * i;
    // set the orientation. By default, the tool will be pointing up into the air when we usually want it to
    // be pointing down into the ground.
    pose *= Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitX()); // this flips the tool around so that Z is down
    pattern_poses.push_back(pose);
  }

  for (int i = num_steps -1; i>=0; --i)
  {
    // Create a pose and initialize it to identity
    Eigen::Affine3d pose = Eigen::Affine3d::Identity();
    // set the translation (we're moving along a line in Y)
    pose.translation() = startPoint + delta * i;
    // set the orientation. By default, the tool will be pointing up into the air when we usually want it to
    // be pointing down into the ground.
    pose *= Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitX()); // this flips the tool around so that Z is down
    pattern_poses.push_back(pose);
  }
}
  // Now lets translate these points to Descartes trajectory points
  Eigen::Affine3d pattern_origin = Eigen::Affine3d::Identity();
  //pattern_origin.translation() = Eigen::Vector3d(0.09, 0, 0.03);

  std::vector<descartes_core::TrajectoryPtPtr> result;
  for (const auto& pose : pattern_poses)
  {
    // This creates a trajectory that searches around the tool Z and let's the robot move in that null space
    descartes_core::TrajectoryPtPtr pt = makeTolerancedCartesianPoint(pattern_origin * pose, time_between_points);
    // This creates a trajectory that is rigid. The tool cannot float and must be at exactly this point.
    //  descartes_core::TrajectoryPtPtr pt = makeCartesianPoint(pattern_origin * pose, time_between_points);
    result.push_back(pt);
  }

  // Step 3: Tell Descartes to start at the "current" robot position
  std::vector<double> start_joints = getCurrentJointState("joint_states");
  descartes_core::TrajectoryPtPtr pt (new descartes_trajectory::JointTrajectoryPt(start_joints));
  result.front() = pt;

  // Note that we could also add a joint point representing the starting location of the robot, or a joint point
  // representing the desired end pose of the robot to the front and back of the vector respectively.

  return result;
}

bool ScanNPlan::executeTrajectory(const trajectory_msgs::JointTrajectory& trajectory)
{
  control_msgs::FollowJointTrajectoryGoal goal;
  goal.trajectory = trajectory;
  goal.goal_time_tolerance = ros::Duration(1.0);
  
  return ac.sendGoalAndWait(goal) == actionlib::SimpleClientGoalState::SUCCEEDED;
}


int main(int argc, char** argv)
{
  ros::init(argc,argv,"myworkcell_node");

  ros::AsyncSpinner async_spinner(1);
  async_spinner.start();

  ros::NodeHandle private_node_handle("~");
  std::string base_frame = "base_link";
  //private_node_handle.param<std::string>("base_frame", base_frame, "world");

  ROS_INFO_STREAM("Base frame: " << base_frame);

  ScanNPlan app;
  if(app.initDescartModel() < 0) {
    return -1;
  }
  ros::Duration(0.5).sleep();
  app.start(base_frame);

  ROS_INFO("ScanNPlan node has been initialized");

  //ros::spin();

  ros::waitForShutdown();
}


