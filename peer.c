#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>

#include "port.h"
#include "ip-conv.h"

#define MSG_NOBLOCK 0x01
#define BUFLEN 		2048
#define MSGS 		1	


struct sockaddr_in myaddr, remaddr;
int connection_count=0;
int fd, i, slen=sizeof(remaddr);
char buf[BUFLEN];	
int recvlen;		
char *server = "54.187.186.32";	

struct credential_t{
	unsigned char publicIP[4];
	unsigned int publicPORT;
}we,they;


unsigned char conneciton_id_validate(unsigned char *, int);
void connect_to_client();
void *connection_read( void *);
/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/

int main(void)
{
	printf("\n\r\n\rConnecting ..");
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}       


	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	sprintf(buf, "Send me my public address and port");
	

	if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {}
	
	while(1) 
	{
		usleep(100);
		
		struct timeval t_v;
		t_v.tv_sec = 0;  t_v.tv_usec = 1; 
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_v,sizeof(struct timeval));
		
		recvlen = recvfrom(fd, buf, BUFLEN, 0,NULL,NULL);
        if (recvlen > 0) 
        {
				connection_count = 0;
				on_receive(buf,recvlen);
        }
        
        if((connection_count++)%500==0)
        {
			printf(".");
			if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {}
		}
        
	}
	close(fd);
	return 0;
}
/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
 void on_receive(unsigned char* buf, unsigned int d_len)
 {
	 if((buf=strstr(buf,"('")) == NULL)
		return;
	 
	 buf += 2;
	 
	 unsigned char myPublicIP[4];
	 unsigned char *tempRef = strstr(buf,"'");
	 unsigned char *tempPrt = strstr(buf,", ");
	 unsigned char message[100]={0};
	 
	 tempRef[0] = NULL;
	 tempPrt += 2;
	 
	 
	 if(ip_str_to_byte(buf, myPublicIP)==0)
		return;
	 
	 memcpy(we.publicIP,myPublicIP,sizeof(we.publicIP));
	 we.publicPORT = atoi(tempPrt);
	 
	 printf("Connected\n\r");
	 
	 sprintf(message,"%d-%d-%d-%d-%d",we.publicIP[0],we.publicIP[1],we.publicIP[2],we.publicIP[3],we.publicPORT);
	 printf("========================================================\n\r");
	 printf("\tYour connection ID is %s\n\r",message);	 
	 printf("========================================================\n\r");
	 
	 do {
		 memset(message,0,sizeof(message));
		 printf("Provide connection ID of other peer: ");
		 scanf("%s",&message);
		 printf("\n\r");
		}while(conneciton_id_validate(message, strlen(message))!=1);
	
	 connect_to_client();
	 
}
/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
unsigned char conneciton_id_validate(unsigned char *connectionID, int lenID)
{
	unsigned char *parser = connectionID;
	
	if((parser=strstr(parser,"-")) == NULL)
		return 0;
	if((parser=strstr(parser,"-")) == NULL)
		return 0;
	if((parser=strstr(parser,"-")) == NULL)
		return 0;
	if((parser=strstr(parser,"-")) == NULL)
		return 0;
	
	parser = connectionID;
	
	they.publicIP[0] = atoi(parser);
	parser=strstr(parser,"-");	
	parser+=1;
	
	they.publicIP[1] = atoi(parser);
	parser=strstr(parser,"-");	
	parser+=1;
	
	they.publicIP[2] = atoi(parser);
	parser=strstr(parser,"-");	
	parser+=1;
	
	they.publicIP[3] = atoi(parser);
	parser=strstr(parser,"-");	
	parser+=1;
	
	they.publicPORT = atoi(parser);
	
	return (they.publicIP[0] > 0 &&
			they.publicIP[1] > 0 &&
			they.publicIP[2] > 0 &&
			they.publicIP[3] > 0 &&
			they.publicPORT > 0);
}
/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
 void connect_to_client()
 {
	
	unsigned char buf[100];
	unsigned char r_server[40];
	
	sprintf(buf, "Hello !!!");

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(they.publicPORT);
	
	
	sprintf(r_server,"%d.%d.%d.%d",they.publicIP[0],they.publicIP[1],they.publicIP[2],they.publicIP[3]);
	
	if (inet_aton(r_server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	
	
	pthread_t thread1;
	pthread_create( &thread1, NULL, connection_read, NULL);

	unsigned char send_msg[1024]={0};
	
	printf("Type Something .. Hit  Enter !! Enjoy\n\r");	
	while(1) 
	{
		usleep(100);
		
		//~ scanf("%s",&send_msg);
		gets(send_msg);
		
		if (sendto(fd, send_msg, strlen(send_msg), 0, (struct sockaddr *)&remaddr, slen)==-1) {}
		memset(send_msg,0,sizeof(send_msg));
	}
 }

/****************************************************************************
 *
 * NAME:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
void *connection_read( void *ptr )
{
	unsigned char buf[1024];
	unsigned char isFirst=0;
	
	while(1)
	{
		usleep(100);
		
		struct timeval t_v;
		t_v.tv_sec = 0;  t_v.tv_usec = 1; 
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_v,sizeof(struct timeval));
		
		recvlen = recvfrom(fd, buf, BUFLEN, 0,NULL,NULL);
        if (recvlen > 0) 
        {
			if(isFirst)
			{
				printf("%s\n\r",buf);
				memset(buf,0,sizeof(buf));
			}
			else
			{
				isFirst=1;
				memset(buf,0,sizeof(buf));
			}
        }
	}
}

