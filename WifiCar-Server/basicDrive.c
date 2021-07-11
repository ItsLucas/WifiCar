#include "basicDrive.h"

void stop() {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,0);
    digitalWrite(BIN2,0);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,0);
}

void front(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,1);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,0);
    digitalWrite(BIN1,1);
    softPwmWrite(PWMB,speed);
    delay(length);
    stop();
}

void back(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,1);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,1);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,speed);
    delay(length);
    stop();
}

void right(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,0);
    digitalWrite(BIN1,1);
    softPwmWrite(PWMB,speed);
    delay(length);
    stop();
}

void left(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,1);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,0);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,0);
    delay(length);
    stop();
}

void back_left(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,0);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,1);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,speed);
    delay(length);
    stop();
}
void back_right(unsigned int speed,unsigned int length) {
    digitalWrite(AIN2,1);
    digitalWrite(AIN1,0);
    softPwmWrite(PWMA,speed);
    digitalWrite(BIN2,0);
    digitalWrite(BIN1,0);
    softPwmWrite(PWMB,0);
    delay(length);
    stop();
}
