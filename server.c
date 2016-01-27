#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS	128
/* Client structure */
typedef struct {
	struct sockaddr_in addr;	/* Client remote address */
	int fd;			/* Connection file descriptor */
	char usrname[32];			/* Client name */
} client_t;

client_t *clients[MAX_CLIENTS];

bool add_client(client_t *client){
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
    if(strcmp(clients[i]->usrname, client->usrname)) {
      return false;
    }
		if(!clients[i]){
			clients[i] = client;
			return true;
		}
	}
  return false;
}

void remove_client(char usrname[32]){
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(clients[i]){
			if(strcmp(clients[i]->usrname, usrname) == 0){
				clients[i] = NULL;
				return;
			}
		}
	}
}

void send_message(char *message, char usrname[32]){
	int i;
  char buffer[255];

	for(i=0;i<MAX_CLIENTS;i++){
		if(clients[i]){
			if(strcmp(clients[i]->usrname, usrname) != 0){
        // concatentate strings to get /MESSAGE/username/message
        bzero(buffer, 256);
        strcat(buffer, clients[i]->usrname);
        strcat(buffer, "/");
        strcat(buffer, message);


				write(clients[i]->fd, message, strlen(message));
			} else {
        write(clients[i]->fd, "/SUCCESS", 8);
      }
		}
	}
}

bool starts_with(const char *pre, const char *str){
    return strncmp(pre, str, strlen(pre)) == 0;
}

void handle_client (int client_fd, struct sockaddr_in client_addr);

int main( int argc, char *argv[] ) {
   int sockfd, client_fd;
   socklen_t client_len;
   struct sockaddr_in server_addr, client_addr;
   int pid;

   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   bzero((char *) &server_addr, sizeof(server_addr));

   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(5000);

   if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }


   listen(sockfd, 5);
   client_len = sizeof(client_addr);

   while (1) {
      client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);

      if (client_fd < 0) {
         perror("ERROR on accept");
         exit(1);
      }

      /* Create child process */
      pid = fork();

      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }

      if (pid == 0) {
         /* This is the client process */
         close(sockfd);
         handle_client(client_fd, client_addr);
         exit(0);
      }
      else {
         close(client_fd);
      }

		sleep(1);

   } /* end of while */

}

void handle_client (int client_fd, struct sockaddr_in client_addr) {
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(client_fd, buffer, 255);

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   if (starts_with("/SIGNUP/", buffer)) {
      client_t *client = (client_t *)malloc(sizeof(client_t));
      client->addr = client_addr;
      client->fd = client_fd;
      strcpy(client->usrname, strtok(buffer, "/SIGNUP/"));
      if (add_client(client)) {
        write(client_fd, "/SUCCESS", 8);
      } else {
        write(client_fd, "/FAILURE", 8);
      }
   } else if (starts_with("/MESSAGE/", buffer)) {
     char *message = strtok(buffer, "/MESSAGE/");
     int i;
     for (i = 0; i < MAX_CLIENTS; i++) {
       if (clients[i]) {
         if (clients[i]->fd == client_fd) {
          send_message(message, clients[i]->usrname);
         }
       }
     }
   } else if (starts_with("/BYE/", buffer)) {
     int i;
     for (i = 0; i < MAX_CLIENTS; i++) {
       if (clients[i]) {
         if (clients[i]->fd == client_fd) {
           clients[i] = NULL;
         }
       }
     }
   } else {
     n = write(client_fd, "/WTF", 4);
   }

   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }

}
