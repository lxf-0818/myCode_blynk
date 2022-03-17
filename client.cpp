#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define PORT "8888"
#define _HOME
//#define _CELL
#ifdef _HOME
#define IP    "10.0.0.67"
#define IP_MB "10.0.0.105"
#endif
#ifdef _CELL
#define IP    "192.168.1.4"
#define IP_MB "192.168.1.3"
#endif

int connect2server(char *ip, char * port,int * sockfd); // connect to rapberry
void error(const char *msg);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
int connect2server(char *ip, char * port,int * sockfd)
{
    int  portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = atoi(port);
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (*sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);
    if (connect(*sockfd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
       error("ERROR connecting");

    printf("connect to server success %s\n",ip);
    return 0;
}

int read_esp8266(char *msg,char * result)
{
    char buffer[256];
    int sockfd,n;
    if (strstr(msg,"DHT"))
     connect2server(IP,PORT,&sockfd);
    else if (strstr(msg,"MB"))
     connect2server(IP_MB,PORT,&sockfd);

    bzero(buffer,strlen(buffer));
    strcpy(buffer,msg);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
   	    error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    //printf("%d from esp %s\n",n,buffer);
    if (n < 0) 
         error("ERROR reading from socket");
    strcpy(result,buffer);
    close(sockfd);
    return 0;
}
