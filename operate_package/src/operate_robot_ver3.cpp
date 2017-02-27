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
int leftMotor[2];
int rightMotor[2];
double moveTime[2];
int moveFlag = 0;



void movementCallBack(const std_msgs::Int16MultiArray msg){
	for(int i = 0; i <= 1; i++){
		go.data=1;
		sign_pub.publish(go);
		
		if(msg.data[2*i] == 0 && msg.data[2*i+1] == 0){
			leftMotor[i] = STOP;
			rightMotor[i] = STOP;
			moveTime[i] = 0;
		}
		
		else if(msg.data[2*i] == 0 && msg.data[2*i+1] != 0){
			switch(msg.data[2*i+1]){
				case -90:
					leftMotor[i] = REVERSE;
					rightMotor[i] = FORWARD;
					moveTime[i] = 4;
					break;
				case 45:
					leftMotor[i] = FORWARD;
					rightMotor[i] = REVERSE;
					moveTime[i] = 2;
					break;
				case -45:
					leftMotor[i] = REVERSE;
					rightMotor[i] = FORWARD;
					moveTime[i] = 2;
					break;
			}
		}
		
		else if(msg.data[2*i] != 0 && msg.data[2*i+1] == 0){
			switch(msg.data[2*i]){
				leftMotor[i] = FORWARD;
				rightMotor[i] = FORWARD;
				case 10:
					moveTime[i] = 2;
					break;
				case 30:
					moveTime[i] = 6;
					break;
			}
		}
	}
	//go.data=1;
	//sign_pub.publish(go);
	moveFlag = 1;
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
	ros::init(argc, argv, "operate_robot");
	ros::NodeHandle nh;
	
	ino_pub = nh.advertise<std_msgs::Int16MultiArray>("send_arduino",1000);
	ino_sub = nh.subscribe("movement",1000,movementCallBack);
	
	sign_pub = nh.advertise<std_msgs::Int16>("go_sign",1000);
	state_sub = nh.subscribe("state",1000,stateCallBack);
	
	while(ros::ok()){
		if(state == 0){
			r_motor.data.clear();
			for(int i = 0;i < 2;i++){
				r_motor.data.push_back(STOP);
			}
			ino_pub.publish(r_motor);
		}
		
		else if(state == 1 && moveFlag == 1){
			for(int i = 0; i <= 1; i++){
				if(i == 0){
					go.data=1;
					sign_pub.publish(go);
				}
				r_motor.data.clear();
				r_motor.data.push_back(leftMotor[i]);
				r_motor.data.push_back(rightMotor[i]);
				if(i == 0)ROS_INFO("To tell that");
				ROS_INFO("  %d, %d",r_motor.data[0],r_motor.data[1]);
				ino_pub.publish(r_motor);
				sleep(moveTime[i]);
				
				r_motor.data.clear();
				r_motor.data.push_back(STOP);
				r_motor.data.push_back(STOP);
				ino_pub.publish(r_motor);
			}
			go.data=1;
			sign_pub.publish(go);
			moveFlag = 0;
		}
		
		ros::spinOnce();
	}
	return 0;
}
