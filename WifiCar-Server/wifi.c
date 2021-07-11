//Author u18301012 2019.07.07
//Simple wifi server to control the pi car.
/*
 *   Packet Definition
 *  F       Drive Forward
 *  B       Drive Backward
 *  L       Turn left, then forward
 *  R       Turn right,then forward
 *  S       Stop the car
 *  AUTO    Enter auto driving mode, using ultrasonic to avoid crashes
 *  Entering any value in F,B,L,R,S terminates the auto driving immediately and go back to wifi mode.
 *  MO1U    Turn left the camera
 *  MO1D    Turn right the camera
 *  MO2U    Turn up the camera
 *  MO2D    Turn down the camera
 * 
 * 
 */
#include "ultra.h"
#include <lirc/lirc_client.h>
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/wait.h>  
#include <sys/time.h>  
#include <sys/types.h>  
  
#define MAXBUF 1024  
  
typedef struct CLIENT {  //Client definition
    int fd;  
    struct sockaddr_in addr;      
}CLIENT;  
 
#define false 0
#define true 1

pthread_t tidr; //Ringing thread
pthread_t tidu; //Ultrasonic thread
pthread_t tidd; //Drivingg thread
struct lirc_config cfg;
char* code;
int flag = 0;
int delay_time=2000;
bool started = false; //Has the threads started?
bool autodriving = false; //Is autodriving
int s1p = 120; //camera servo left/right pos
int s2p = 0; //camera servo up/down pos
int speed = 50;//speed to change
bool bell = false;

typedef struct dr {
    char itn; //intention. f,b,l,r
    bool avl; //availability
}driver;

driver driv;

void auto_drive() { //Function for auto driving
    pwm_write(0,90);
    float dis = 0.0f;
    while(1) {
        dis=disMeasure();
        delay_time=(int)dis * 20;
        if(dis<30.0f) {
            stop();
            bool l = measure_left();
            bool f = measure_front();
            bool r = measure_right();
            printf("l:%d f:%d r:%d\n",l,f,r);                
            pwm_write(0,90);
            if((f==0&&l==0)&&r==0) {
                front(45,100);
            }   
            else if((f==0&&l==0)&&r==1){
                front(45,100);
            }
            else if((f==0&&l==1)&&r==0){
                front(45,100);
            }
            else if((f==0&&l==1)&&r==1){
                front(45,100);
            }
            else if((f==1&&l==0)&&r==0){
                ld>rd?left(50,600):right(50,600);
            }
            else if((f==1&&l==0)&&r==1){
                left(50,600);
            }
            else if((f==1&&l==1)&&r==0){
                right(50,600);
            }       
            else if((f==1&&l==1)&&r==1){
                back(80,1000);
            }
            else {
                front(50,50);
            }
        }
        else {
            front(40,100);
        }
    }
}

void ring_task() {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    for(;;) {
        if(bell) {
            softToneWrite(BuzPin,CH3);
            delay(200);
            softToneWrite(BuzPin,CH1);
            delay(200);
            softToneWrite(BuzPin,0);  
        }
        else if(delay_time<2000) {
            softToneWrite(BuzPin,600);
            delay(200);
            softToneWrite(BuzPin,0);
            delay(min(1000,delay_time));
            //printf("%d\n",delay_time);
        }
        else {
            softToneWrite(BuzPin,0);
        }
    }
}
//Three threads, responsible for ringing,driving,ultrasonic probing
pthread_t new_ring_task() {
    pthread_t ring_thread;
    pthread_create(&ring_thread,NULL,(void *)&ring_task,NULL);
    printf("Ring thread started\n");
    return ring_thread;
}

void ultra_task() {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    for(;;) {
        switch(driv.itn) {
            case 'f':
                driv.avl = measure_front();break;
            case 'l':
                driv.avl = measure_left();break;
            case 'r':
                driv.avl = measure_right();break;
            case 'b':
                driv.avl = 0;break;
            default:break;
        }
    }
}

pthread_t new_ultra_task() {
    pthread_t ultra_thread;
    pthread_create(&ultra_thread,NULL,(void *)&ultra_task,NULL);
    printf("Ultra thread started\n");
    return ultra_thread;
}

void drive_task() {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    for(;;) {
        switch(driv.itn) {
            case 'f':
                {
                    if(driv.avl==0) {
                        delay_time=2000;
                        front(speed,70);
                    }
                    else {
                        stop();
                        delay_time=500;
                    }
                    break;
                }
            case 'l':
                {
                    if(driv.avl==0) {
                        left(50,800);
                        driv.itn='f';
                        printf("Intention changed to %c\n",driv.itn);
                    }
                    else {
                        stop();
                        delay_time=500;
                    }
                    break;
                }
            case 'r':
                {
                    if(driv.avl==0) {
                        right(50,800);
                        driv.itn='f';
                        printf("Intention changed to %c\n",driv.itn);
                    }
                    else {
                        stop();
                        delay_time=500;
                    }
                    break;
                }
            case 'b':
                {
                    back(speed,70);
                }
                break;
            case 'c':
                {
                    auto_drive();
                }
            default:stop();break;
        }
    }
}

pthread_t new_drive_task() {
    pthread_t drive_thread;
    pthread_create(&drive_thread,NULL,(void *)&drive_task,NULL);
    printf("Drive thread started\n");
    return drive_thread;
}

//Control the car according to packets recieved via wifi.
void wificontrol(char* c) {
    if(strstr(c,"F")) {//f
        driv.itn='f';
        if(autodriving==true) {
            tidu = new_ultra_task();
            pthread_cancel(tidd);
            tidd = new_drive_task();
            autodriving=false;
        }
        //printf("Intention changed to %c\n",driv.itn);
    }
    else if(strstr(c,"L")) {//l
        driv.itn='l';
        if(autodriving==true) {
            tidu = new_ultra_task();
            pthread_cancel(tidd);
            tidd = new_drive_task();
            autodriving=false;
        }
        //printf("Intention changed to %c\n",driv.itn);
    }
    else if(strstr(c,"S")) {//s
        driv.itn='s';
        if(autodriving==true) {
            tidu = new_ultra_task();
            pthread_cancel(tidd);
            tidd = new_drive_task();
            autodriving=false;
        }
        //printf("Intention changed to %c\n",driv.itn);
    }
    else if(strstr(c,"R")) { //r
        driv.itn='r';
        if(autodriving==true) {
            tidu = new_ultra_task();
            pthread_cancel(tidd);
            tidd = new_drive_task();
            autodriving=false;
        }
        //printf("Intention changed to %c\n",driv.itn);
    }
    else if(strstr(c,"B")) { //b
        driv.itn='b';
        if(autodriving==true) {
            tidu = new_ultra_task();
            pthread_cancel(tidd);
            tidd = new_drive_task();
            autodriving=false;
        }
        //printf("Intention changed to %c\n",driv.itn);
    }
    else if(strstr(c,"AUTO")) {
        driv.itn='c';
        printf("Auto driving...\n");
        //pthread_cancel(tidr);
        if(autodriving==false) {
            pthread_cancel(tidu);
            autodriving=true;
        }
        //pthread_cancel(tidd);

    }
}
//Control the camera
void cameracontrol(char* c) {
    if(strstr(code,"MO1U")) {
        s1p+=10;
        if(s1p>180) s1p=180;
        pwm_write(1,s1p);
    }
    else if(strstr(code,"MO1D")) {
        s1p-=10;
        if(s1p<0) s1p=0;
        pwm_write(1,s1p);
    }
    else if(strstr(code,"MO2U")) {
        s2p-=10;
        if(s2p<0) s2p=0;
        pwm_write(2,s2p);
    }
    else if(strstr(code,"MO2D")) {
        s2p+=10;
        if(s2p>180) s2p=180;
        pwm_write(2,s2p);
    }
    else if(strstr(code,"MOK")) {
        s1p=120;
        s2p=0;
        pwm_write(1,s1p);
        pwm_write(2,s2p);
    }
}
/*************************** 
**Main server implementation  
**Event-based TCP server 
****************************/  
int main(int argc, char** argv)  
{  
    init(); //init the car
    pwm_write(0,90); //reset the ultrasonic serno
    if(lirc_init("lirc",1)==-1) {
        printf("Failed to init lirc\n");
        return 0;
    }
    if(lirc_readconfig(NULL,&cfg,NULL)!=0) {
        printf("Failed to read config\n");
        return 0;
    }
    int i,n,maxi = -1;  
	int j=0;
    int nready;  
    int slisten,sockfd,maxfd=-1,connectfd;
	int maxifd=-1;
    int sendn=-1;
	char sendbuf[100]="helloworld";
    unsigned int myport,lisnum;   
    CLIENT client[FD_SETSIZE];  
    struct sockaddr_in  my_addr,addr;  
    struct timeval tv;  
      
    socklen_t len;  
    fd_set rset,allset,wset;   
      
    char buf[MAXBUF + 1];  
     
  
    myport = 2333;  //port for listening
  
    lisnum = 5;  
  
    if((slisten = socket(AF_INET,SOCK_STREAM,0)) == -1) {  
        perror("socket");  
        exit(1);  
    }  
  
    bzero(&my_addr,sizeof(my_addr));  
    my_addr.sin_family = AF_INET;  
    my_addr.sin_port = htons(myport);  
    my_addr.sin_addr.s_addr = INADDR_ANY;  
	char snd_buf[128]="helloworld";
    if(bind(slisten, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {  
        perror("bind");  
        exit(1);  
    }  
  
    if (listen(slisten, lisnum) == -1) {  
        perror("listen");  
        exit(1);  
    }  
  
    for(i=0;i<5;i++) {  
        client[i].fd = -1;  
    }  
 
	printf("Waiting for connections and data...\n");
    while (1) {         
        tv.tv_sec = 1;  
        tv.tv_usec = 0;
		FD_ZERO(&allset);             
		FD_SET(slisten, &allset);    
		maxfd = slisten;  		
		if(select(maxfd+1,&allset,NULL,NULL,&tv)>0) {
			len = sizeof(struct sockaddr);  
			if((connectfd = accept(slisten,(struct sockaddr*)&addr,&len)) == -1) {  
				perror("accept() error\n");  
			}
			for(i=0;i<5;i++) {
				if(client[i].fd<0) {
					client[i].fd=connectfd;
					break;
				}
			}
			if(i==5) {
				printf("too many connect\n");
			}
		}
		for(j=0;j<5;j++) {
			if(client[j].fd<0)
				continue;
			FD_ZERO(&rset);
			FD_SET(client[j].fd,&rset);
			FD_ZERO(&wset);
			FD_SET(client[j].fd,&wset);	
			maxifd = client[j].fd;				
			if(select(maxifd + 1, &rset,&wset, NULL, &tv)>0) {
			    if(FD_ISSET(client[j].fd,&rset))   {                         
					bzero(buf,MAXBUF + 1);  
					if((n = recv(client[j].fd,buf,MAXBUF,0)) > 0) {  
                        printf("Data1:%s \n",buf);
						
                        if(started==false) { //start threads and drive
                            driv.itn = 'a';
                            driv.avl = true;
                            tidr = new_ring_task();
                            tidu = new_ultra_task();
                            tidd = new_drive_task();
                            started=true;
                        }
                        code = buf;
                        if(strstr(code,"CU")) {
                            speed+=10;
                            if(speed>70) speed=70;
                            printf("Current speed:%d\n",speed);
                        }
                        else if(strstr(code,"CD")) {
                            speed-=10;
                            if(speed < 0) speed=0;
                            printf("Current speed:%d\n",speed);
                        }
                        else if(strstr(code,"BELL")) {
                            bell=!bell;
                        }
                        else if(strstr(code,"MO")) {
                            cameracontrol(code);
                        }
                        else {
                            wificontrol(code); 
                        }
                        if(flag==1) goto shutdown;  //terminate
                        
                        //printf("Data1:%s \n",buf);
					}                 
				}
				if(FD_ISSET(client[j].fd,&wset)) {
                    //printf("Data2:%s \n",buf);
                    //printf("Data3:%s \n",snd_buf);
					//n = send(client[j].fd,snd_buf,128,0) > 0 ;
				}
			}
		}			
		//sleep(1);
    }  
shutdown: 
    stop();
    close(slisten);  
    //free(&code);
    //lirc_freeconfig(&cfg);
    lirc_deinit();
    pthread_cancel(tidr);
    pthread_cancel(tidu);
    pthread_cancel(tidd);
    stop();
    return 0;
} 
