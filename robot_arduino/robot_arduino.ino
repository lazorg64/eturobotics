/*
 * rosserial PubSub Example
 * Prints "hello world!" and toggles led
 */

#include <ros.h>
#include <AFMotor.h>
#include <std_msgs/String.h>
ros::NodeHandle nh;
AF_DCMotor motor(1);
char input[15];
void messageCb( const std_msgs::String& toggle_msg){
  input[0] = toggle_msg.data[0];

}
ros::Subscriber<std_msgs::String> sub("to_ard", messageCb );
std_msgs::String str_msg;
ros::Publisher chatter("from_ard", &str_msg);






void setup()
{       
  motor.setSpeed(255);
  motor.run(RELEASE);
  memset(input,0,15);
  input[0] = 'a';
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.advertise(chatter);
  nh.subscribe(sub);
}

void loop()
{
  switch(input[0])
  {
    case 'w':
    motor.run(FORWARD);
    break;
    
    case 's':
    motor.run(BACKWARD);
    break;
    
    case 'q':
    motor.run(RELEASE);
    break;
    
  }

  str_msg.data = input;
  chatter.publish( &str_msg );
  nh.spinOnce();
  delay(50);
}
