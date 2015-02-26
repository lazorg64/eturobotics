#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

void chatterCallback(const std_msgs::String::ConstPtr& msg) {
 ROS_INFO("I heard: [%s]", msg->data.c_str());
}
     
int main(int argc, char **argv) {
 ros::init(argc, argv, "connector");
 ros::NodeHandle n;
 ros::Subscriber sub = n.subscribe("from_ard", 1000, chatterCallback);
 ros::Publisher chatter_pub = n.advertise<std_msgs::String>("to_ard", 1000);
 ros::Rate loop_rate(500);
 while (ros::ok()) {
	 
  std_msgs::String msg;
  chatter_pub.publish(msg);
  ros::spin();
  loop_rate.sleep();
  
 }
 return 0; 
}


