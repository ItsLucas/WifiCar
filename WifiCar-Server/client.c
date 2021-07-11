//Author u18301012 2019.07.07
//Simple packet sender.
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
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <resolv.h>  
#include <stdlib.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include <sys/types.h>  
  
#define MAXBUF 1024  
#define PORT 2333
int main(int argc, char **argv)  
{  
        int sockfd, len;  
        struct sockaddr_in dest;  
        char buf[MAXBUF + 1];  
        fd_set rfds,wfds;  
        struct timeval tv;  
		int retval, maxfd = -1;  
		char sendbuf[128]="helloworld";
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
            perror("Socket");  
            exit(errno);  
        }  
		char *seraddr="127.0.0.1";
		bzero(&dest, sizeof(dest));  
        dest.sin_family = AF_INET;  
        dest.sin_port = htons(PORT);  
        if (inet_aton(seraddr, (struct in_addr *) &dest.sin_addr.s_addr) == 0) {  
            perror(seraddr);  
            exit(errno);  
        }  
  
		if(connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {  
            perror("Connect ");  
            exit(errno);  
        }  
  
		printf("connect to server...\n");         
		while (1)   
		{  
  
                FD_ZERO(&rfds);             
                FD_SET(sockfd, &rfds);  
				FD_ZERO(&wfds);             
                FD_SET(sockfd, &wfds); 
                maxfd = 0;  
              
                //FD_SET(sockfd, &rfds);  
                scanf("%s",sendbuf);
                if (sockfd > maxfd)  
                    maxfd = sockfd;  
              
                tv.tv_sec = 1;  
                tv.tv_usec = 0;  
            
                    retval = select(maxfd + 1, &rfds, &wfds, NULL, &tv);  
  
				if (retval == -1)   
				{  
                        printf("select error! %s", strerror(errno));                
						break;  
                } 
				else if (retval == 0) 
				{   
                        continue;  
                } 
				else 
				{  
					if (FD_ISSET(sockfd, &wfds))   
					{                  
                            //bzero(buf, MAXBUF + 1);  
                            len = send(sockfd, sendbuf, strlen(sendbuf), 0);  
                            if (len > 0)  
                                printf("msg:%s send successful，totalbytes: %d！\n", sendbuf, len);  
                            else {  
                                printf("msg:'%s  failed!\n", sendbuf);  
                                break;  
                            }  
							
                    }  
                    if(FD_ISSET(sockfd, &rfds))   
					{   
                                bzero(buf, MAXBUF + 1);  
                                len = recv(sockfd, buf, MAXBUF, 0);  
                                if (len > 0)  
                                    printf("recv:'%s, total: %d \n", buf, len);  
                                else    
								{  
									if (len < 0)   
                                            printf("recv failed！errno:%d，error msg: '%s'\n", errno, strerror(errno));  
                                    else  
                                            printf("other exit，terminal chat\n");  
                                    break;  
								}  
                    }                 
            } 
			sleep(1);
    }  
  
    close(sockfd);  
    return 0;  
}  
