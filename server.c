#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "support.h"

int main(int argc, char *argv[]){
	int server_fd = 0;
  struct sockaddr_in server_addr;
  int client_fd = 0;
	struct sockaddr_in client_addr;
  int pid;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(5000);

	if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Error on bind");
		return 1;
	}

	if(listen(server_fd, 1) < 0){
		perror("Error on listen");
		return 1;
	}

	printf("Server Up:\n");
  printf("----------\n");
  printf("IP: %s\n", addr_to_string(server_addr));
  printf("Port: 5000\n\n");

	while(1) {
    socklen_t client_addr_len = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);

    // Check if too many clients
		if ((CLIENT_COUNT+1) == MAX_CLIENTS) {
			printf("Error: Too many clients. Rejecting %s\n", addr_to_string(client_addr));
			close(client_fd);
			continue;
		}

    // Make new client
		client_data *client = (client_data *)malloc(sizeof(client_data));
		client->addr = client_addr;
		client->fd = client_fd;
		client->id = ID_DISPATCH++;
		sprintf(client->username, "%d", client->id);
		add_client(client);

    // Fork and handle client
    pid = fork();
    if (pid < 0) {
       perror("Error on fork");
       exit(1);
    }

    if (pid == 0) { // child
      close(server_fd);
      handle_client(client);
      exit(0);
    } else { // parent
      close(client_fd);
    }

    sleep(1);
	}
}
