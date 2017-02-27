#include <ros.h>
#include <std_msgs/Int16.h>

ros::NodeHandle nh;


void messageCb(const std_msgs::Int16& msg) {
	if (msg.data ==1) digitalWrite(13, HIGH);
	else digitalWrite(13, LOW);
}

ros::Subscriber<std_msgs::Int16>sub("confirm", messageCb);
void setup() {
	pinMode(13,OUTPUT);
	nh.initNode();
	nh.subscribe(sub);
}
void loop() {
	nh.spinOnce();
	delay(10);
}
