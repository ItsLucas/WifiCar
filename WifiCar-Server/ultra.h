//Author u18301012 2019.07.02
//Contains basic ultrasonic stuff definitions.
#include "tone.h"
#include "basicDrive.h"
#include "pca9685.h"
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#define Trig 28
#define Echo 29
#define BtnPin 24
#define PIN_BASE 300
#define HERTZ 50
#define MAX_PWM 4096 //for servo
#define max(x,y) ((x)>(y)?(x):(y)) //for angle correction
#define min(x,y) ((x)<(y)?(x):(y))

float ld,rd;

typedef int bool;

int calcTicks(float ms,int hertz);
void pwm_write(int servo,float x);
float disMeasure();
bool measure_front();
bool measure_left();
bool measure_right();
void init(); //total init of the car