#include <ros/ros.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;

    cv_bridge::CvImage cv_image1, cv_image2;
    cv_image1.image = cv::imread("image1.jpg",CV_LOAD_IMAGE_COLOR);
    cv_image2.image = cv::imread("image2.jpg",CV_LOAD_IMAGE_COLOR);
    cv_image1.encoding = "bgr8";
    cv_image2.encoding = "bgr8";
    sensor_msgs::Image ros_image1, ros_image2;
    cv_image1.toImageMsg(ros_image1);
    cv_image2.toImageMsg(ros_image2);

  ros::Publisher pub = nh.advertise<sensor_msgs::Image>("/static_image", 1);
  ros::Rate loop_rate(30);

  while (nh.ok()) 
  {
    for(int i = 0; i < 15; i++) {
      pub.publish(ros_image1);
      loop_rate.sleep();
      ros::spinOnce();
    }
    for(int i = 0; i < 15; i++) {
      pub.publish(ros_image2);
      loop_rate.sleep();
      ros::spinOnce();
    }
  }
}
