#include "ros/ros.h"
#include <visualization_msgs/Marker.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/String.h"
#include "std_msgs/UInt32.h"
#include <sstream>
#include <iostream>
#include <string>

#define MAX_POINTS 20000  //Maximum points to display

typedef pcl::PointCloud<pcl::PointXYZI> PointCloud;  //create a pointcloud using XYZ and I(ntensity)



PointCloud * pcmsg;
int counter;

void chatterCallback(const std_msgs::UInt32::ConstPtr& msg) {

     std::cout << msg->data<< std::endl;
}
     
int main(int argc, char **argv) {
 ros::init(argc, argv, "connector");
 ros::NodeHandle n;

 counter = 0;

 pcmsg = new PointCloud();
 pcmsg->header.frame_id = "base_sonar";
 pcmsg->height = 1;
 pcmsg->width = MAX_POINTS;
 pcmsg->points.resize (pcmsg->width * pcmsg->height);
 std::cout << "Point Cloud created..." << std::endl;


 ros::Subscriber sub = n.subscribe("from_ard", 1000, chatterCallback);
 ros::Publisher chatter_pub = n.advertise<std_msgs::String>("to_ard", 1000);
 ros::Rate loop_rate(10);

 std::cout << "ready" << std::endl;

 ros::AsyncSpinner spinner(4); // Use 4 threads
 spinner.start();

 while (ros::ok()) {
  std_msgs::String output;
  std::string str;
  std::cin >> str;
  output.data = str.c_str();
  chatter_pub.publish(output);

  loop_rate.sleep();
  
 }
 ros::waitForShutdown();
 return 0; 
}


