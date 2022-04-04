/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ostream>
#include <iostream>
#include <fstream>

using namespace std;
void connection_info(struct sockaddr_in &client);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void connection_info(struct sockaddr_in &client){
    	
    char *connected_ip= inet_ntoa(client.sin_addr);
    printf("Connected From %s \n",connected_ip);

}
int main(int argc, char *argv[])
{
	
    int sockfd, newsockfd, portno,n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
  
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    else printf("socket open success\n"); 

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1){

     	newsockfd = accept(sockfd, 
        	         (struct sockaddr *) &cli_addr, 
                	 &clilen);

     	if (newsockfd < 0) error("ERROR on accept");

		connection_info(cli_addr);

     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);

     	if (n < 0) error("ERROR reading from socket");
		ofstream myfile;
		if (strstr(buffer,"DHT"))
    		myfile.open ("/home/pi//blynk-library/linux/ip_dht.txt");
		else
    		myfile.open ("/home/pi//blynk-library/linux/ip_adc.txt");
		myfile << buffer+4;
		myfile.close();
			

     	n = write(newsockfd,"I got your message",18);
     	if (n < 0) error("ERROR writing to socket");
     	close(newsockfd);
     	printf("close socket\n");
     }
     return 0; 
}
