#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>

#include <string.h>
#include "main.h"


const char *BINARY_PATH="$HOME/youtube-dl";
char *ARGS[] = {&BINARY_PATH, "-f", "best", "-o", "$HOME/Videos/%(uploader)s%(title)s.%(ext)s", "", NULL};
int PORT_NUMBER=5001;

void doprocessing (int sock) {
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("Here is the message: %s\n",buffer);

   ARGS[(sizeof(ARGS)/sizeof(char*))-2] = buffer;

   int code = execv(BINARY_PATH, ARGS);
   n = write(sock,"I got your message: " + code, 18);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
	
}

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int pid, n;

   if (argc < 2) {
     fprintf( stderr, "Pass port numbert to program arguments\n" );
     exit(1);
   }

   int port_num = argv[1];
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port_num);
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      pid = fork();
		
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
    
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
   }
}
