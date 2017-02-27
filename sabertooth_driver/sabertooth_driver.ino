
/******************************************
  2016.2.22　森 剛史

  Sabertoothのモータードライバーを動かす


******************************************/

#include <SoftwareSerial.h>

SoftwareSerial driver1(2,3);
SoftwareSerial driver2(4,5);
SoftwareSerial driver3(6,7);

// 足動かす関数
void motor_go(int length, int duty);
void motor_turn(int angle, int duty);
void motor_stop(void);
int duty_limit(int duty);


void setup() {
  // put your setup code here, to run once:
  // パラメータ
  int driver_baudrate = 9600;

  // 変更不可
  Serial.begin(9600);
  driver1.begin(driver_baudrate);
  driver2.begin(driver_baudrate);
  driver3.begin(driver_baudrate);
}

void loop() {
  // put your main code here, to run repeatedly:

  motor_go(1000, 100);
  
}

void motor_go(int length, int duty){
  duty = duty_limit(duty);
  int speed = 63 * duty / 100;
  int vector;
  if(length >= 0)   vector = +1;
  else              vector = -1;
  driver1.write( 64 + vector * speed);
  driver1.write(192 - vector * speed);
  driver2.write( 64 + vector * speed);
  driver2.write(192 - vector * speed);
  driver3.write( 64 + vector * speed);
  driver3.write(192 - vector * speed);

  int time = abs(length);
  delay(time);
  motor_stop();
}

void motor_turn(int angle, int duty){
  duty = duty_limit(duty);
  int speed = 63 * duty / 100;
  int vector;
  if(angle >= 0)   vector = +1;
  else             vector = -1;
  driver1.write( 64 + vector * speed);
  driver1.write(192 + vector * speed);
  driver2.write( 64 + vector * speed);
  driver2.write(192 + vector * speed);
  driver3.write( 64 + vector * speed);
  driver3.write(192 + vector * speed);

  int time = abs(angle);
  delay(time);
  motor_stop();
}

void motor_stop(void){
  driver1.write( 64);
  driver1.write(192);
  driver2.write( 64);
  driver2.write(192);
  driver3.write( 64);
  driver3.write(192);
}

int duty_limit(int duty){
  if(duty > 100)    return 100;
  else if(duty < 0) return 0;
  else              return duty;
}

