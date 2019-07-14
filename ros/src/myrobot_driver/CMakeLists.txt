cmake_minimum_required(VERSION 2.8.3)
project(myrobot_driver)
add_definitions(-std=c++11)

# let actionlib pull in all other dependencies
find_package(catkin REQUIRED COMPONENTS roscpp actionlib joint_state_controller)

catkin_package(
  CATKIN_DEPENDS actionlib joint_state_controller
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(myrobot_action_server  src/myrobot_action_server.cpp)
target_link_libraries(myrobot_action_server 
  ${catkin_LIBRARIES})
  
#install(TARGETS
#  myrobot_action_server
#  LIB DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#)
