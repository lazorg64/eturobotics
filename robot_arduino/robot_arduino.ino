/*
 * rosserial PubSub Example
 * Prints "hello world!" and toggles led
 */

#include <ros.h>
#include <AFMotor.h>
#include <std_msgs/String.h>
#include <std_msgs/UInt32.h>

#include <Servo.h> 
#include "Ultrasonic.h"

#define CM 1

Ultrasonic front(19, 18);
Ultrasonic back(14, 15);
Ultrasonic left(16, 17);
Ultrasonic right(2, 3);

ros::NodeHandle nh;

AF_DCMotor motor(4);
AF_DCMotor motor2(1);
Servo myservo; 
char input[15];


std_msgs::UInt32 int_msg;
ros::Publisher chatter("from_ard", &int_msg);


void messageCb( const std_msgs::String& toggle_msg){
  input[0] = toggle_msg.data[0];
  int angle = 90;
  char* ptr = 0;
  switch(input[0])
  {
    case 'w':
    forward(1000);
    break;
    case 's':
    backward(1000);
    break;
    case 'a':
    for(int i=0;i<15;i++)
      input[i] = toggle_msg.data[i];
    ptr = &input[1];
    sscanf(ptr,"%d",&angle);
    myservo.write(angle);
    break;
    case 't':
    int_msg.data = front.Ranging(CM);
    chatter.publish( &int_msg );
    break;
    case 'b':
    int_msg.data = back.Ranging(CM);
    chatter.publish( &int_msg );
    break;
    case 'l':
    int_msg.data =left.Ranging(CM);
    chatter.publish( &int_msg );
    break;
    case 'r':
    int_msg.data = right.Ranging(CM);
    chatter.publish( &int_msg );
    break; 
  }
  
}
ros::Subscriber<std_msgs::String> sub("to_ard", messageCb );

void forward(int time)
{
  motor.run(RELEASE);
  motor2.run(RELEASE);
  motor.run(FORWARD);
  motor2.run(FORWARD);
  delay(time);
  motor.run(RELEASE);
  motor2.run(RELEASE);  
}

void backward(int time)
{
  motor.run(RELEASE);
  motor2.run(RELEASE);
  motor.run(BACKWARD);
  motor2.run(BACKWARD);
  delay(time);
  motor.run(RELEASE);
  motor2.run(RELEASE);  
}


void setup()
{       
  motor.run(RELEASE);
  motor2.run(RELEASE);
  motor.setSpeed(80);
  motor2.setSpeed(80);
  myservo.attach(10);
  myservo.write(90);
  memset(input,0,15);
  input[0] = 'o';
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.advertise(chatter);
  nh.subscribe(sub);  
}

void loop()
{

  
  nh.spinOnce();
  delay(100);
}
