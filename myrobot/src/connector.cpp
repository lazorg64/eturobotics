#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <iostream>
#include <string>

void chatterCallback(const std_msgs::String::ConstPtr& msg) {
 ROS_INFO("I heard: [%s]", msg->data.c_str());
}
     
int main(int argc, char **argv) {
 ros::init(argc, argv, "connector");
 ros::NodeHandle n;
 ros::Subscriber sub = n.subscribe("from_ard", 1000, chatterCallback);
 ros::Publisher chatter_pub = n.advertise<std_msgs::String>("to_ard", 1000);
 ros::Rate loop_rate(10);
 while (ros::ok()) {
	 
  std_msgs::String output;
  std::cin >> output.data;
  chatter_pub.publish(output);
  ros::spinOnce();
  loop_rate.sleep();
  
 }
 return 0; 
}


