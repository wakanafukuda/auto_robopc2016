#include <stdio.h>
#include <stdlib.h>

#include "ros/ros.h"

#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"

#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/Int16.h"
#include "std_msgs/String.h"

#define STOP 0
#define START 1

#define FORWARD 50
#define REVERSE -50

ros::Publisher ino_pub;
ros::Publisher sign_pub;

ros::Subscriber ino_sub;
ros::Subscriber state_sub;

std_msgs::Int16 go;
std_msgs::Int16MultiArray r_motor;
static int state = 0;
double moveTime = 0;



void movementCallBack(const std_msgs::Int16MultiArray msg){
	if(state == 0){
		r_motor.data.clear();
		for(int i = 0;i < 2;i++){
			r_motor.data.push_back(STOP);
		}
		ino_pub.publish(r_motor);
	}
  
	else if(state == 1){
		//go.data=1;
		//sign_pub.publish(go);
		for(int j = 0; j <= 1; j++){
			r_motor.data.clear();
			
			if(msg.data[2*j] == 0 && msg.data[2*j+1] == 0){
				r_motor.data.push_back(STOP);
				r_motor.data.push_back(STOP);
				moveTime = 0;
			}
			
			else if(msg.data[2*j] == 0 && msg.data[2*j+1] != 0){
				switch(msg.data[2*j+1]){
					case -90:
						r_motor.data.push_back(REVERSE);
						r_motor.data.push_back(FORWARD);
						moveTime = 2;
						break;
					case 45:
						r_motor.data.push_back(FORWARD);
						r_motor.data.push_back(REVERSE);
						moveTime = 1;
						break;
					case -45:
						r_motor.data.push_back(REVERSE);
						r_motor.data.push_back(FORWARD);
						moveTime = 1;
						break;
				}
			}
			
			else if(msg.data[2*j] != 0 && msg.data[2*j+1] == 0){
				switch(msg.data[2*j]){
					case 10:
						r_motor.data.push_back(FORWARD);
						r_motor.data.push_back(FORWARD);
						moveTime = 1;
						break;
					case 30:
						r_motor.data.push_back(FORWARD);
						r_motor.data.push_back(FORWARD);
						moveTime = 2;
						break;
				}
			}

			ROS_INFO("%d, %d",r_motor.data[0],r_motor.data[1]);
			ino_pub.publish(r_motor);
			sleep(moveTime);
			r_motor.data.clear();
			r_motor.data.push_back(STOP);
			r_motor.data.push_back(STOP);
			ino_pub.publish(r_motor);
			sleep(1);
		}
		go.data=1;
		sign_pub.publish(go);
	}
}



void stateCallBack(const std_msgs::String::ConstPtr& msg){
	if( (msg->data) == "stop"){
		state = 0;
	}
	else if( (msg->data) == "start"){
		state = 1;
	}
	ROS_INFO("I heard : [ %s ]", msg->data.c_str());
	ROS_INFO("I heard : [ %d ]", state);
}



int main(int argc, char** argv){
	ros::init(argc, argv, "operate_robot_ver2");
	ros::NodeHandle nh;
	
	ino_pub = nh.advertise<std_msgs::Int16MultiArray>("send_arduino",1000);
	ino_sub = nh.subscribe("movement",1000,movementCallBack);
	
	sign_pub = nh.advertise<std_msgs::Int16>("go_sign",1000);
	state_sub = nh.subscribe("state",1000,stateCallBack);
	
	while(ros::ok()){
		ros::spinOnce();
	}
	return 0;
}
