#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Checks is a string starts with another */
bool starts_with(const char *pre, const char *str){
    return strncmp(pre, str, strlen(pre)) == 0;
}

int main(int argc, char *argv[]) {
   int sockfd, port, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

   char buffer[256];
   char usrname_buffer[30];
   char message_buffer[232];

   /* START set up connection to server */
   if (argc < 3) {
      fprintf(stderr,"usage %s host port\n", argv[0]);
      exit(0);
   }

   port = atoi(argv[2]);
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   server = gethostbyname(argv[1]);

   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(port);

   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   /* END set up connection to server */

   /* START choose username */
   printf("Choose a username: ");
   bzero(usrname_buffer, 31); //clear username_buffer
   fgets(usrname_buffer, 30, stdin); //read from stdin
   bzero(buffer,256); // clear buffer
   strcpy(buffer, "/SIGNUP/"); // copy /SIGNUP/ into buffer
   strcat(buffer, usrname_buffer); //concatentate buffer and username to get /SIGNUP/username

   n = write(sockfd, buffer, strlen(buffer));

  if (n < 0) {
    perror("ERROR writing to socket");
    exit(1);
  }

  // Get response from server
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);

  if (n < 0) {
    perror("ERROR reading from socket");
    exit(1);
  }


  if(strcmp("/SUCCESS", buffer) == 0) { //if successful
    while(1) {
      // Send Message
      printf("Compose: ");
      bzero(buffer,256);
      bzero(message_buffer,232);
      fgets(message_buffer,255,stdin);

      strcpy(buffer, "/MESSAGE/");
      strcat(buffer, message_buffer);

      n = write(sockfd, buffer, strlen(buffer));

      if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
      }

      /* Check for response */
      bzero(buffer,256);
      n = read(sockfd, buffer, 255);

      if (n < 0) {
         perror("ERROR reading from socket");
         exit(1);
      }

      // Read from server for new messages
      while(1) {
        bzero(buffer,256);
        n = read(sockfd, buffer, 255);

        if (n < 0) {
           perror("ERROR reading from socket");
           exit(1);
        }

        if(starts_with("/MESSAGE/", buffer)) {
          // TODO: parse message /MESSAGE/username/message...
        }
      }
      printf("%s\n",buffer);
    }
  } else {
    printf("Username was taken or the room is full, try again later...\n");
  }
  /* END choose username */


  return 0;
}
