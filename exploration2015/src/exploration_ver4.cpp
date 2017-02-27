#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <std_msgs/Int16.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/String.h>

#define TO_GO_INTO_A_MAZE -1
#define FIRST_TURN 0
#define FIRST_WALL_CHECK 1
#define SECOND_TURN 2
#define SECOND_WALL_CHECK 3
#define THIRD_TURN 4
#define THIRD_WALL_CHECK 5
#define FOURTH_TURN 6
#define FIFTH_TURN 7
#define DECIDE_DIRECTION 8
#define GO 9
#define RESET 10

#define LEFT 0
#define FORWARD_ONE_STEP 1
#define FORWARD_TWO_STEPS 2

#define ONE_STEP 30
#define TWO_STEPS 60

#define MIN_DISTANCE 850
#define DANGEROUS_DISTANCE 350

int state = -1;
int roboMove = 0;
double deepth[6];
double ave[3];
int moveForward[2];
int moveTurn[2];

ros::Subscriber sign_sub;
ros::Publisher motor_pub;



void aveArea(const cv::Mat& img, float& ave_1, float& ave_2, float& ave_3, float& ave_4) {
	int sum_1=0,sum_2=0,sum_3=0,sum_4=0,rows=img.rows,cols=img.cols;
        
	for(int i=0;i<(rows/2);i++){
		for(int j=0;j<(cols/2);j++){
			sum_1+=img.at<unsigned short>(i,j);
     			
		}
	}

	for(int i=rows/4;i<(rows/2);i++){
		for(int j=(cols/2);j<cols;j++){
			sum_2+=img.at<unsigned short>(i,j);		
			
		}
	}
	for(int i=(rows/2);i<rows;i++){
		for(int j=0;j<(cols/2);j++){
		 	sum_3+=img.at<unsigned short>(i,j);		
			
		}
	}
	for(int i=(rows/2);i<rows*3/4;i++){
		for(int j=(cols/2);j<cols;j++){
			sum_4+=img.at<unsigned short>(i,j);		
			
		}
	}
	
	ave_1=sum_1/((rows/2)*(cols/2));
	ave_2=sum_2/((rows/2)*(cols/2));
	ave_3=sum_3/((rows/2)*(cols/2));
	ave_4=sum_4/((rows/2)*(cols/2));
	
}



void depth_callback(const sensor_msgs::ImageConstPtr& msg) {
	cv_bridge::CvImagePtr cv_ptr;
	
	try {
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);
	}

	catch (cv_bridge::Exception& e) {
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	
	cv::imshow("test", cv_ptr->image);
	cv::waitKey(1);
	
	
	if (state == FIRST_WALL_CHECK) {
		float ave1, ave2, ave3, ave4;
		aveArea(cv_ptr->image, ave1, ave2, ave3, ave4);                                                                
		deepth[0] = ave3;
		deepth[1] = ave4;
		ave[0] = (ave3 + ave4) / 2;
		ROS_INFO("AREA[1]=%lf",deepth[0]);
		ROS_INFO("AREA[2]=%lf",deepth[1]);
		state++;
	}
	
	if (state == SECOND_WALL_CHECK) {
		float ave1, ave2, ave3, ave4;
		aveArea(cv_ptr->image, ave1, ave2, ave3, ave4);
		deepth[2] = ave3;
		deepth[3] = ave4;
		ave[1] = (ave3 + ave4) / 2;
		ROS_INFO("AREA[3]=%lf",deepth[2]);
		ROS_INFO("AREA[4]=%lf",deepth[3]);
		state++;
	}
	
	if (state == THIRD_WALL_CHECK) {
		float ave1, ave2, ave3, ave4;
		aveArea(cv_ptr->image, ave1, ave2, ave3, ave4);
		deepth[4] = ave3;
		deepth[5] = ave4;
		ave[2] = (ave3 + ave4) / 2;
		ROS_INFO("AREA[5]=%lf",deepth[4]);
		ROS_INFO("AREA[6]=%lf",deepth[5]);
		state++;
	}
}



void goSignCallback(const std_msgs::Int16 sign){
	state++;
}




int main(int argc, char** argv) {
	ros::init(argc, argv, "exploration_node");
	ros::NodeHandle nh;
	
	ros::Rate loop_rate(1);
	
	image_transport::ImageTransport it(nh);
	image_transport::Subscriber image_sub = it.subscribe("/camera/depth/image_raw", 1 ,depth_callback);
	
	sign_sub = nh.subscribe<std_msgs::Int16>("go_sign", 1000, goSignCallback);
	
	motor_pub = nh.advertise<std_msgs::Int16MultiArray>("movement",1000);
	
	std_msgs::Int16MultiArray moveMotor;
	
	
	while(ros::ok()) {
		switch(state){
			case TO_GO_INTO_A_MAZE:
				ROS_INFO("To go into a maze");
				moveForward[0] = 30;  moveTurn[0] = 0;
				moveForward[1] = 0;  moveTurn[1] = 0;
				break;
		
			case FIRST_TURN:
				ROS_INFO("The state is the first turn");
				moveForward[0] = 0;  moveTurn[0] = -90;
				moveForward[1] = 0;  moveTurn[1] = 0;
				break;

			case SECOND_TURN:
				if(state == SECOND_TURN)
					ROS_INFO("The state is the second turn");
			case FOURTH_TURN:
				if(state == FOURTH_TURN)
					ROS_INFO("The state is the fourth turn");
				moveForward[0] = 0;  moveTurn[0] = 45;
				moveForward[1] = 0;  moveTurn[1] = 0;
				break;

			case THIRD_TURN:
				if(state == THIRD_TURN)
					ROS_INFO("The state is the third turn");
				moveForward[0] = 0;  moveTurn[0] = 44;
				moveForward[1] = 0;  moveTurn[1] = 0;
				break;

			case FIFTH_TURN:
				ROS_INFO("The state is the fifth turn");
				if(1.2*deepth[1] < deepth[0] || ave[0] < DANGEROUS_DISTANCE){
					ROS_INFO("30 degree turn");
					moveTurn[0] = -30;
				}
				else if(deepth[0] < 0.8*deepth[1]){
					ROS_INFO("60 degree turn");
					moveTurn[0] = -60;
				}
				else{
					ROS_INFO("45 degree turn");
					moveTurn[0] = -45;
				}
				moveForward[0] = 0;
				moveForward[1] = 0;  moveTurn[1] = 0;
				break;
			
				
			case DECIDE_DIRECTION:
				ROS_INFO("The state is to decide direction");
				if(ave[1] < 1.2*1.414*ave[0] && ave[0] < MIN_DISTANCE && MIN_DISTANCE < ave[2]){
					roboMove = FORWARD_TWO_STEPS;
					ROS_INFO("To take two steps");
				}
				else if(0.8*1.414*ave[0] < ave[1] && ave[0] < MIN_DISTANCE && MIN_DISTANCE < ave[2]){
					roboMove = FORWARD_ONE_STEP;
					ROS_INFO("To take one step");
				}
				else if(MIN_DISTANCE < ave[0]){
					roboMove = LEFT;
					ROS_INFO("To turn to left and take two steps");
				}
				else if(ave[0] < MIN_DISTANCE && ave[1] < MIN_DISTANCE*1.414 && ave[2] < MIN_DISTANCE){
					state = 0;
					ROS_INFO("To turn to right and redo");
				}
				else{
					state = -1;
					ROS_INFO("To reserch again");
				}
				state++;
				break;
				
			case GO:
				ROS_INFO("The state is to go");
				switch(roboMove){
					case LEFT:
						moveForward[0] = 0;  moveTurn[0] = -91;
						moveForward[1] = TWO_STEPS;  moveTurn[1] = 0;
						break;
		
					case FORWARD_ONE_STEP:
						moveForward[0] = ONE_STEP;  moveTurn[0] = 0;
						moveForward[1] = 0;  moveTurn[1] = 0;
						break;
			
					case FORWARD_TWO_STEPS:
						moveForward[0] = TWO_STEPS;  moveTurn[0] = 0;
						moveForward[1] = 0;  moveTurn[1] = 0;
						break;
				}
				break;
			
			case RESET:
				ROS_INFO("The state is to reset");
				moveForward[0] = 0;  moveTurn[0] = 0;
				moveForward[1] = 0;  moveTurn[1] = 0;
				state = 0;
				break;
		}
		
		moveMotor.data.clear();
		for(int i = 0; i <= 1 && (moveForward[0] != 0 || moveTurn[0] != 0); i++){
			moveMotor.data.push_back(moveForward[i]);
			moveMotor.data.push_back(moveTurn[i]);
			if(i == 0)  ROS_INFO("To tell that");
			ROS_INFO("  %d, %d",moveMotor.data[2*i], moveMotor.data[2*i+1]);
			if(i == 1)
				motor_pub.publish(moveMotor);
		}
		moveForward[0] = 0;  moveTurn[0] = 0;
		moveForward[1] = 0;  moveTurn[1] = 0;
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
