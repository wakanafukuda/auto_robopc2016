#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <std_msgs/Int16.h>
int jadge=0;

void aveArea(const cv::Mat &img, float &ave_1, float &ave_2, float &ave_3, float &ave_4) {
  int sum_1_r=0,sum_2_r=0,sum_3_r=0,sum_4_r=0,rows=img.rows,cols=img.cols;

	for(int i=0;i<(rows/2);i++){
		for(int j=0;j<(cols/2);j++){
		  cv::Vec3b intensity =img.at<cv::Vec3b>(j,i);
			 uchar red_color=intensity(2);
			sum_1_r+=red_color;						
		}
	}

	for(int i=0;i<(rows/2);i++){
		for(int j=(cols/2);j<cols;j++){
		  cv::Vec3b intensity =img.at<cv::Vec3b>(j,i);
			uchar red_color=intensity(2);
			sum_2_r+=red_color;	
		}
	}
	for(int i=(rows/2);i<rows;i++){
		for(int j=0;j<(cols/2);j++){
		  cv::Vec3b intensity =img.at<cv::Vec3b>(j,i);
			uchar red_color=intensity(2);
			sum_3_r+=red_color;
		}
	}
	for(int i=(rows/2);i<rows;i++){
		for(int j=(cols/2);j<cols;j++){
		  cv::Vec3b intensity =img.at<cv::Vec3b>(j,i);
			uchar red_color=intensity(2);
			sum_4_r+=red_color;		
		}
	}

	ave_1=sum_1_r/((rows/2)*(cols/2));
	ave_2=sum_2_r/((rows/2)*(cols/2));
	ave_3=sum_3_r/((rows/2)*(cols/2));
	ave_4=sum_4_r/((rows/2)*(cols/2));
}		


void thermal_callback(const sensor_msgs::ImageConstPtr& msg) {	
	cv_bridge::CvImagePtr cv_ptr;
	try {
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}

	catch (cv_bridge::Exception& e) {
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	cv::imshow("test2", cv_ptr->image);
	cv::waitKey(1);
		float ave1, ave2, ave3, ave4;
		aveArea(cv_ptr->image, ave1, ave2, ave3, ave4);                                                                    
                float red=(ave1+ave2+ave3+ave4);
		ROS_INFO("RED_VALUE=%lf",red);
		if(red>700){
	         ROS_INFO("VICTIM!");
		jadge=1;
	  }
	
}
	
int main(int argc, char** argv) {
	ros::init(argc, argv, "thermal_node");
	ros::NodeHandle nh;
	image_transport::ImageTransport it(nh);
	image_transport::Subscriber image_sub = it.subscribe("/optris/thermal_image_view", 1 ,thermal_callback);
	ros::Publisher ino_pub =nh.advertise<std_msgs::Int16>("confirm", 1000);
	ros::Rate loop_rate(1);
       
	while(ros::ok()) {
		std_msgs::Int16 msg;
		msg.data=jadge;
		ino_pub.publish(msg);
		loop_rate.sleep();
		ros::spinOnce();
	}
	return 0;
}

