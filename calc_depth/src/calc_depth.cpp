/*
0. �p�b�P�[�W�̍쐬
$ catkin_create_pkg calcDepth cv_bridge image_transport roscpp sensor_msgs std_msgs

1. Kinect����摜���擾
$ roslaunch openni_launch openni.launch

2. image_view ��rviz�ŉ摜������
*/
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//�Ԙg�̑傫��(���肷��͈�)
//���ۂ̉�f���́A(WIDTH*2)*(HEIGHT*2)
#define WIDTH   50
#define HEIGHT  25


class depth_estimater{
public:
	depth_estimater();//�R���X�g���N�^
	~depth_estimater();
	void rgbImageCallback(const sensor_msgs::ImageConstPtr& msg);//RGB�摜���w�ǂ������ɌĂ΂��֐�
	void depthImageCallback(const sensor_msgs::ImageConstPtr& msg);//Depth�摜���w�ǂ������ɌĂ΂��֐�

private:
	ros::NodeHandle nh;
	ros::Subscriber sub_rgb, sub_depth;
};

depth_estimater::depth_estimater(){
	sub_rgb = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_color", 1, &depth_estimater::rgbImageCallback, this);
	sub_depth = nh.subscribe<sensor_msgs::Image>("/camera/depth/image", 1, &depth_estimater::depthImageCallback, this);
}

depth_estimater::~depth_estimater(){
}
 
void depth_estimater::rgbImageCallback(const sensor_msgs::ImageConstPtr& msg){

	int i, j;
	int x1, x2, y1, y2;
	int width = WIDTH;
	int height = HEIGHT;
	cv_bridge::CvImagePtr cv_ptr;//�摜���������߂̂���

/*
	�摜���w�ǂ��ABGR8�`��(1�`�����l��(�A�΁A�Ԃ̂ǂꂩ1��)�ɂ�8�r�b�g(0�`255�K��))
	�̃J���[�摜�ɕϊ����Acv_ptr�ɑ�����Ă���B�����Œ��ӂ��ׂ��_�́A�`�����l���̏��Ԃ�
	BGR���Ȃ킿�A�΁A�Ԃ̏��ɂȂ��Ă���_�B�摜���w�ǂł��Ȃ��ꍇ�́A�G���[���o�́B
*/

	try{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}catch (cv_bridge::Exception& ex){
		ROS_ERROR("error");
		exit(-1);
	}

	cv::Mat &mat = cv_ptr->image;
	//�Ԙg�̑傫����ݒ�
	x1 = int(mat.cols / 2) - width;
	x2 = int(mat.cols / 2) + width;
	y1 = int(mat.rows / 2) - height;
	y2 = int(mat.rows / 2) + height;

/*	�Ԙg���̉�f�ɃA�N�Z�X���A�Ɨ΂̃`�����l����0�������Ă���B
	�`�����l���̏��Ԃ͐A�΁A�ԁB��f�ɃA�N�Z�X���Ă���v���O������3�s���邪
	�A��قǂ̃`�����l���̏��ԂƂȂ��Ă���̂Œ��ӁB��f�ɃA�N�Z�X������@��
	����������B
*/
	for(i = y1; i < y2; i++){
		for(j = x1; j < x2; j++){
			// 0 : blue, 1 : green, 2 : red.
			mat.data[i * mat.step + j * mat.elemSize() + 0] = 0;
			mat.data[i * mat.step + j * mat.elemSize() + 1] = 0;
			//mat.data[i * mat.step + j * mat.elemSize() + 2] = 0;
		}
	}

	//rectangle�֐��Řg�̊O����ԐF�ň͂��Ă���B
	cv::rectangle(cv_ptr->image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 3, 4);
	cv::imshow("RGB image", cv_ptr->image);//imshow�֐��ɂ��摜��\��
/*	waitKey�֐��̓L�[���������܂őҋ@����֐��B����̓L�[���͂�K�v�Ƃ��Ȃ����A
	�摜��\������ꍇ�ɂ͕K�v�Ȋ֐��Ȃ̂ŋL�q����B
*/
	cv::waitKey(10);
}

void depth_estimater::depthImageCallback(const sensor_msgs::ImageConstPtr& msg){

	int x1, x2, y1, y2;
	int i, j, k;
	int width = WIDTH;
	int height = HEIGHT;
	double sum = 0.0;
	double ave;
	cv_bridge::CvImagePtr cv_ptr;

	//TYPE_32FC1�́A1�`�����l�����������_��(1�̉�f��32�r�b�g)��\�킷�B
	
	try{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
	}catch (cv_bridge::Exception& ex){
		ROS_ERROR("error");
		exit(-1);
	}
	//CV_8UC1�́A��قǏo�Ă���BGR8�Ɠ����B
	cv::Mat depth(cv_ptr->image.rows, cv_ptr->image.cols, CV_32FC1);
	cv::Mat img(cv_ptr->image.rows, cv_ptr->image.cols, CV_8UC1);

	x1 = int(depth.cols / 2) - width;
	x2 = int(depth.cols / 2) + width;
	y1 = int(depth.rows / 2) - height;
	y2 = int(depth.rows / 2) + height;

/*	�g���̋����l�̍��v���v�Z���Ă���B���̍ہA����������0[m]�ȉ��ł���΁A���O�B
	CV_32FC1�^�ł́Adata���\�b�h���g�p���ĉ�f�ɃA�N�Z�X���邱�Ƃ��ł��Ȃ����߁A
	ptr���\�b�h���g�p���Ă���B�܂��A�����摜���������邽�߂ɁA����
	�l���擾�ł���ő勗���Ŋ���A255���|���Ă���B255��8�r�b�g�ŕ\���ł���ő��
	���B�Ō�ɁA�g���̉�f���Ŋ���A�v�Z�������ϋ��������̍s�ŏo�͂��Ă���B
*/
	for(i = 0; i < cv_ptr->image.rows;i++){
		float* Dimage = cv_ptr->image.ptr<float>(i);
		float* Iimage = depth.ptr<float>(i);
		char* Ivimage = img.ptr<char>(i);
		for(j = 0 ; j < cv_ptr->image.cols; j++){
			if(Dimage[j] > 0.0){
				Iimage[j] = Dimage[j];
				Ivimage[j] = (char)(255*(Dimage[j]/5.5));
			}else{
			}

			if(i > y1 && i < y2){
				if(j > x1 && j < x2){
					if(Dimage[j] > 0.0){
						sum += Dimage[j];
					}
				}
			}
		}
	}

	ave = sum / ((width * 2) * (height * 2));
	ROS_INFO("depth : %f [m]", ave);

	cv::imshow("DEPTH image", img);
	cv::waitKey(10);
}

int main(int argc, char **argv){
	ros::init(argc, argv, "depth_estimater");

	depth_estimater depth_estimater;

	ros::spin();
	return 0;
}