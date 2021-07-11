//Author u18301012
//Contains basic car movements (front,back,left,right) and engine defs.
#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>

#define PWMA 1
#define AIN2 2
#define AIN1 3
#define PWMB 4
#define BIN2 5
#define BIN1 6

void stop();
void front(unsigned int speed,unsigned int length);
void back(unsigned int speed,unsigned int length);
void right(unsigned int speed,unsigned int length);
void left(unsigned int speed,unsigned int length);
void back_left(unsigned int speed,unsigned int length);
void back_right(unsigned int speed,unsigned int length);