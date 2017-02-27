#include <stdio.h>
#include <stdlib.h>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <std_msgs/Int16.h>
#include <msg2015/CmdPWM.h>

#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"

#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/Int16.h"
#include "std_msgs/String.h"

#define FRONT 0
#define LEFT 1
#define RIGHT 2
#define BACK 3

#define STOP 0
#define START 1
#define PAUSE 2

#define FORWARD 50
#define REVERSE -50

#define TIMEBASE 15

const double thrsholds = 800.0;
int servo_state = 0;//0: front, 1: left, 2: right, 3: back
int robot_state = 0;//0: front, 1: left, 2: right, 3: back
int button_state = 0;//0: stop, 1: start, 2: pause
double image_ave = 0;
std_msgs::Int16MultiArray r_motor;
std_msgs::Int16 servo_msg;

ros::Publisher ino_pub;
ros::Publisher servo_pub;
ros::Subscriber state_sub;

void aveArea(const cv::Mat& img, float& ave){
	int i,j;
	int rows = img.rows;
	int cols = img.cols;
	int sum = 0;

	for(i = 0;i < rows;i++){
		for(j = 0;j < cols;j++){
			sum += img.at<unsigned short>(i,j);
		}
	}
	ave = sum / (rows * cols);
}

void depth_callback(const sensor_msgs::ImageConstPtr& msg){
	int i;
	float ave;
	cv_bridge::CvImagePtr cv_ptr;
	try{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);
	}catch(cv_bridge::Exception& e){
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}

	aveArea(cv_ptr->image, ave);
	image_ave = ave;
	ROS_INFO("average: %f",image_ave);
}		

void state_callback(const std_msgs::String::ConstPtr& msg){
	if( (msg->data) == "stop"){
		button_state = STOP;
	}else if( (msg->data) == "start"){
		button_state = START;
	}else if( (msg->data) == "pause"){
		button_state == PAUSE;
	}
	ROS_INFO("I heard: [ %s ]", msg->data.c_str());
}

int main(int argc, char** argv){
	ros::init(argc, argv, "ex2016");
	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
	image_transport::Subscriber image_sub = it.subscribe("/camera/depth/image_raw", 1, depth_callback);
	int i;

	ino_pub = n.advertise<std_msgs::Int16MultiArray>("send_arduino",1000);
	servo_pub = n.advertise<std_msgs::Int16>("arm_pan",1000);
	state_sub = n.subscribe("state",1000,state_callback);

	servo_msg.data = 90;
	servo_pub.publish(servo_msg);

	while(ros::ok()){
		if(button_state == STOP || button_state == PAUSE){
			r_motor.data.clear();
			for(i = 0;i < 2;i++){
				r_motor.data.push_back(STOP);
			}
			ino_pub.publish(r_motor);
			sleep(1);
		}else if(button_state == START){
			if(image_ave >= thrsholds){
				if(servo_state == FRONT){
					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						r_motor.data.push_back(FORWARD);
					}
					ino_pub.publish(r_motor);
					sleep(1);

					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						r_motor.data.push_back(STOP);
					}
					ino_pub.publish(r_motor);
				}else if(servo_state == LEFT){
					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						if(i == 0){
							r_motor.data.push_back(REVERSE);
						}else if(i == 1){
							r_motor.data.push_back(FORWARD);
						}
					}
					ino_pub.publish(r_motor);
					sleep(3);

					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						r_motor.data.push_back(STOP);
					}
					ino_pub.publish(r_motor);
					servo_state == FRONT;
					servo_msg.data = 90;//サーボを元の向きに戻す
					servo_pub.publish(servo_msg);
					sleep(1);
				}else if(servo_state == RIGHT){
					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						if(i == 0){
							r_motor.data.push_back(FORWARD);
						}else if(i == 1){
							r_motor.data.push_back(REVERSE);
						}
					}
					ino_pub.publish(r_motor);
					sleep(3);

					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						r_motor.data.push_back(STOP);
					}
					ino_pub.publish(r_motor);
					servo_state == FRONT;
					servo_msg.data = 90;//サーボを元の向きに戻す
					servo_pub.publish(servo_msg);
					sleep(1);
				}else if(servo_state == BACK){
					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						if(i == 0){
							r_motor.data.push_back(REVERSE);
						}else if(i == 1){
							r_motor.data.push_back(FORWARD);
						}
					}
					ino_pub.publish(r_motor);
					sleep(8);

					r_motor.data.clear();
					for(i = 0;i < 2;i++){
						r_motor.data.push_back(STOP);
					}
					ino_pub.publish(r_motor);
					servo_state == FRONT;
					sleep(1);
				}
			}else{
				robot_state++;
				if(robot_state == LEFT){
					servo_msg.data = 0;//サーボを左に
					servo_pub.publish(servo_msg);
					servo_state = LEFT;
					sleep(1);
				}else if(robot_state == RIGHT){
					servo_msg.data = 180;//サーボを右に
					servo_pub.publish(servo_msg);
					servo_state = RIGHT;
					sleep(1);
				}else if(robot_state == BACK){
					servo_state = BACK;
					robot_state = FRONT;
				}
			}
		}
	ros::spinOnce();
	}
	return 0;
}
