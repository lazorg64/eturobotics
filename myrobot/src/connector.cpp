#include "ros/ros.h"
#include <visualization_msgs/Marker.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>
#include <tf/transform_datatypes.h>
#include <tf/tfMessage.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/String.h"
#include "std_msgs/UInt32.h"
#include <sstream>
#include <sstream>
#include <iostream>
#include <string>

#define MAX_POINTS 20000

typedef pcl::PointCloud<pcl::PointXYZI> PCloud;

tf::Vector3 base_link_pos;


struct ultrasonic_scan{
    unsigned int front;
    unsigned int back;
    unsigned int left;
    unsigned int right;
};

ultrasonic_scan last_scan;


int counter;

char direction;
char state;
int moveangle;



bool sync_resp;

float scale = 0.1f;

ros::Publisher chatter_pub;

void chatterCallback(const std_msgs::UInt32::ConstPtr& msg) {
//std::cout << "Callback!" << msg->data<< std::endl;

    if(direction == 't')
    {
        last_scan.front = (unsigned int)msg->data;
    }
    else if(direction == 'b')
    {
        last_scan.back = (unsigned int)msg->data;
    }
    else if(direction == 'l')
    {
        last_scan.left = (unsigned int)msg->data;
    }
    else if(direction == 'r')
    {
        last_scan.right = (unsigned int)msg->data;
    }
    else{
        std::cout << "dir = " << direction << "data:"<< msg->data<< std::endl;
    }

    std::cout <<direction << " = "<< msg->data<< std::endl;
    sync_resp = true;
}


void getSensorsData()
{

    std::string str;
    std_msgs::String output;

    sync_resp = false;
    str = "t";
    direction=str.c_str()[0];
    output.data = str.c_str();
    chatter_pub.publish(output);
    //std::cout << "front" << std::endl;
    while(!sync_resp)
    {
    usleep(10000);
    }
    sync_resp = false;
    str = "b";
    direction=str.c_str()[0];
    output.data = str.c_str();
    chatter_pub.publish(output);
    //std::cout << "back" << std::endl;
    while(!sync_resp)
    {
    usleep(10000);
    }
    sync_resp = false;
    str = "r";
    direction=str.c_str()[0];
    output.data = str.c_str();
    chatter_pub.publish(output);
    while(!sync_resp)
    {
        usleep(10000);
    }
    sync_resp = false;
    str = "l";
    direction=str.c_str()[0];
    output.data = str.c_str();
    chatter_pub.publish(output);
    while(!sync_resp)
    {
        usleep(10000);
    }
    sync_resp = false;


}

void sendChar(std::string str)
{
    std_msgs::String output;
    output.data = str.c_str();
    chatter_pub.publish(output);
}

std::string intToString (int Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

void rotAngle(int angle)
{
    std::string str;
    str = "a " + intToString(angle);
    std_msgs::String output;
    output.data = str.c_str();
    chatter_pub.publish(output);
}

void move()
{
    if(last_scan.front>20)
    {
        state = 'w';
        sendChar("w");
        usleep(100000);
        if(last_scan.right<20)
        {
            rotAngle(80);
        }
        else if(last_scan.right>20)
        {
            rotAngle(100);
        }
        else
        {
            rotAngle(90);
        }
    }
    else
    {
        rotAngle(30);
        usleep(1000000);
        rotAngle(90);

    }



}



     
int main(int argc, char **argv) {
 ros::init(argc, argv, "connector");
 ros::NodeHandle n;
 direction = 't';
 counter = 0;

 ros::Subscriber sub = n.subscribe("from_ard", 280, chatterCallback);
 chatter_pub = n.advertise<std_msgs::String>("to_ard", 280);



 base_link_pos = tf::Vector3(0,0,0);
 tf::TransformBroadcaster broadcaster;



 ros::Rate loop_rate(10);


 std::cout << "ready" << std::endl;

 usleep(1000000);
 ros::AsyncSpinner spinner(4);
 spinner.start();
 usleep(1000000);

 state = 'q';

 std::string str;
 std_msgs::String output;
 str = "c";
 output.data = str.c_str();
 chatter_pub.publish(output);
 usleep(1000000);

 int spinCount = 100;

 while (ros::ok()) {


  base_link_pos = base_link_pos + tf::Vector3(0.01,0,0);
  broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), base_link_pos),ros::Time::now(),"map", "base_link"));
  broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.16, 0, 0)),ros::Time::now(),"base_link","front"));
  broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(-0.16, 0, 0)),ros::Time::now(),"base_link","back"));
  broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, 0.06, 0)),ros::Time::now(),"base_link","left"));
  broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, -0.06, 0)),ros::Time::now(),"base_link","right"));
  std::cout << "Spin!" << std::endl;



  getSensorsData();
  std::cout << "Sensors!" << std::endl;
  move();

  std::cout << "Moved!" << std::endl;

  spinCount --;
  if(spinCount<=0)
  {
      state = 'q';
      sendChar("q");
      exit(0);
  }
  loop_rate.sleep();


 }
 //ros::waitForShutdown();
 return 0; 
}


