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
#include "geometry_msgs/Polygon.h"
#include "geometry_msgs/Point32.h"
#include "geometry_msgs/PolygonStamped.h"
#include <sstream>
#include <sstream>
#include <iostream>
#include <string>

//#define MAX_POINTS 20000

//typedef pcl::PointCloud<pcl::PointXYZI> PCloud;

tf::Vector3 base_link_pos;

struct ultrasonic_scan{
    unsigned int front;
    unsigned int back;
    unsigned int left;
    unsigned int right;
};

ultrasonic_scan last_scan;

float speedConst = 0.01;

int counter;

char direction;
char state;
int moveangle;



bool sync_resp;

float scale = 20.0f;

ros::Publisher chatter_pub;
ros::Publisher polygon_pub;

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
    if(last_scan.front>25)
    {
        state = 'w';
        sendChar("w");
        usleep(10000);
        float ang =std::max(std::min(90-2*(25-(int)last_scan.right),150),30);
        rotAngle(ang);

        //float radAngle = (ang-90)*M_PI/180;




        //tf::Vector3 tmp = base_link_pos + tf::Vector3(speedConst*cos(radAngle),speedConst*sin(radAngle),0);

        //base_link_pos = tmp;
    }
    else
    {
        rotAngle(30);
        //float radAngle = (30-90)*M_PI/180;
        //base_link_pos = base_link_pos + tf::Vector3(3*speedConst*cos(radAngle),3*speedConst*sin(radAngle),0);
        usleep(1000000);
    }
}
     
int main(int argc, char **argv) {
 ros::init(argc, argv, "connector");
 ros::NodeHandle n;
 direction = 't';
 counter = 0;

 ros::Subscriber sub = n.subscribe("from_ard", 280, chatterCallback);
 chatter_pub = n.advertise<std_msgs::String>("to_ard", 280);

 polygon_pub = n.advertise<geometry_msgs::PolygonStamped>("polygons",1000);



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

 broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), base_link_pos),ros::Time::now(),"map", "base_link"));
 broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.16, 0, 0)),ros::Time::now(),"base_link","front"));
 broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(-0.16, 0, 0)),ros::Time::now(),"base_link","back"));
 broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, 0.06, 0)),ros::Time::now(),"base_link","left"));
 broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, -0.06, 0)),ros::Time::now(),"base_link","right"));


 while (ros::ok()) {



broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), base_link_pos),ros::Time::now(),"map", "base_link"));
broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.16, 0, 0)),ros::Time::now(),"base_link","front"));
broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(-0.16, 0, 0)),ros::Time::now(),"base_link","back"));
broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, 0.06, 0)),ros::Time::now(),"base_link","left"));
broadcaster.sendTransform(tf::StampedTransform(tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, -0.06, 0)),ros::Time::now(),"base_link","right"));

  getSensorsData();

  geometry_msgs::Polygon pol;
  geometry_msgs::Point32 tl;
  geometry_msgs::Point32 tr;
  geometry_msgs::Point32 bl;
  geometry_msgs::Point32 br;

  tl.x = (float)last_scan.front/scale;
  tl.y = (float)last_scan.left/scale;
  tr.x = (float)last_scan.front/scale;
  tr.y = -(float)last_scan.right/scale;
  bl.x = -(float)last_scan.back/scale;
  bl.y = (float)last_scan.left/scale;
  br.x = -(float)last_scan.back/scale;
  br.y = -(float)last_scan.right/scale;

  pol.points.push_back(tl);
  pol.points.push_back(tr);
  pol.points.push_back(br);
  pol.points.push_back(bl);

  geometry_msgs::PolygonStamped stamped_msg;

  stamped_msg.header = std_msgs::Header();
  stamped_msg.header.frame_id = "base_link";

  stamped_msg.polygon = pol;


  polygon_pub.publish(stamped_msg);




  move();

  spinCount --;
  if(spinCount<=0)
  {
      state = 'q';
      sendChar("q");
      usleep(1000000);
      exit(0);
  }
  loop_rate.sleep();


 }
 return 0; 
}


