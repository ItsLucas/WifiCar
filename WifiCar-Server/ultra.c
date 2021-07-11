//Author u18301012 2019.07.02
//Contains implementations of ultrasonic-related stuff.
#include "ultra.h"
int calcTicks(float ms,int hertz) {
    float cycle = 1000.0f/hertz;
    return (int)(MAX_PWM * ms / cycle +0.5f);
}

void pwm_write(int servo,float x) {
    float y;
    int tick;
    y=x/90.0+0.5;
    y=max(y,0.5);
    y=min(y,2.5);
    tick=calcTicks(y,HERTZ);
    pwmWrite(PIN_BASE+servo,tick);
}

float disMeasure() { //measure the distance
    struct timeval tv1;
    struct timeval tv2;
    long start,stop;
    float dis;
    digitalWrite(Trig,LOW);
    delayMicroseconds(2);
    digitalWrite(Trig,HIGH);
    delayMicroseconds(11);
    digitalWrite(Trig,LOW);
    while(!(digitalRead(Echo)==1));
    gettimeofday(&tv1,NULL);
    while(!(digitalRead(Echo)==0));
    gettimeofday(&tv2,NULL);
    start=tv1.tv_sec * 1000000 + tv1.tv_usec;
    stop=tv2.tv_sec * 1000000 + tv2.tv_usec;
    dis=(float)(stop-start)/1000000*34000/2;
    return dis;
}
//measure distance in 3 directions
bool measure_front() {
    pwm_write(0,90);
    delay(300);
    float d = disMeasure();
    //printf("fd:%0.2f ",d);
    return d < 30.0f;
}

bool measure_left() {
    pwm_write(0,170);
    delay(300);
    float d = disMeasure();
    //printf("ld:%0.2f ",d);
    ld = d;
    return d < 30.0f;
}

bool measure_right() {
    pwm_write(0,10);
    delay(300);
    float d = disMeasure();
    //printf("rd:%0.2f\n",d);
    rd = d;
    return d < 30.0f;
}

void init() {
    int fd = pca9685Setup(PIN_BASE,0x40,HERTZ);
    if(fd<0) {
        printf("Error\n");
        return 0;
    }
    pca9685PWMReset(fd);
    if(wiringPiSetup()==-1) {
        printf("wiringPi setup failed!\n");
        return -1;
    }
    pinMode(BuzPin,OUTPUT);
    softToneCreate(BuzPin);
    pinMode(PWMA,OUTPUT);
    pinMode(AIN2,OUTPUT);
    pinMode(AIN1,OUTPUT);
    pinMode(PWMB,OUTPUT);
    pinMode(BIN2,OUTPUT);
    pinMode(BIN1,OUTPUT);
    pinMode(BtnPin,INPUT);
    softPwmCreate(PWMA,0,100);
    softPwmCreate(PWMB,0,100);
    pinMode(Trig,OUTPUT);
    pinMode(Echo,INPUT);
}