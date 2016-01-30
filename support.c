#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "support.h"

/**
 * Adds a client to the queue
 * @param client The client_data to add to the queue
 */
void add_client(client_data *client) {
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(!CLIENTS[i]){
			CLIENTS[i] = client;
			CLIENT_COUNT++;
			return;
		}
	}
}

/**
 * Removes a client from the queue
 * @param id The id of the client to be removed
 */
void remove_client(client_data *client) {
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(CLIENTS[i]){
			if(CLIENTS[i]->id == client->id){
				CLIENTS[i] = NULL;
				CLIENT_COUNT--;
				return;
			}
		}
	}
}

/**
 * Handle all input and output for clients
 * @param  client The client to 'handle'
 */
void *handle_client(client_data *client) {
	char output[1024];
	char input[1024];
	int len;

	printf("New client with id: %d and address: %s\n", client->id, addr_to_string(client->addr));

	sprintf(output, "(server): %s joined the room\n", client->username);
	send_all(output);

	while((len = read(client->fd, input, sizeof(input)-1)) > 0){
	  input[len] = '\0';
	  output[0] = '\0';
		strip_newline(input);

		if(!strlen(input)){
			continue;
		}

		/* Commands */
		if(input[0] == '('){
			char *command, *param;
			command = strtok(input," ");
			if(!strcmp(command, "(LEAVE)")){
				break;
			} else if(!strcmp(command, "(PING)")) {
				send_client("(server): pong\n", client);
			} else if(!strcmp(command, "(USERNAME)")) {
				param = strtok(NULL, " ");
				if(param){
					char *old_username = strdup(client->username);
					strcpy(client->username, param);
					sprintf(output, "(server): %s changed to %s\n", old_username, client->username);
					free(old_username);
					send_all(output);
				}else{
					send_client("(server): Error new username cannot be empty\n", client);
				}
			} else if(!strcmp(command, "(DM)")) {
				param = strtok(NULL, " ");
				if(param) {
					int id = atoi(param);
					param = strtok(NULL, " ");
					if(param){
						sprintf(output, "(server): Direct message from (%s)", client->username);
						while(param != NULL){
							strcat(output, " ");
							strcat(output, param);
							param = strtok(NULL, " ");
						}
						strcat(output, "\n");
						int i;
						for(i=0;i<MAX_CLIENTS; i++){
							if(CLIENTS[i]){
								if(CLIENTS[i]->id == id){
									send_client(output, CLIENTS[i]);
								}
							}
						}

					} else {
						send_client("(server): Error message can't be empty\n", client);
					}
				} else {
					send_client("(server): Error tried to send message to nonexistant user\n", client);
				}
			} else if(!strcmp(command, "(ONLINE)")) {
				sprintf(output, "(server): %d people online\n", CLIENT_COUNT);
				send_client(output, client);
				list_online(client);
			} else if(!strcmp(command, "(HELP)")){
				strcat(output, "(USERNAME)	<username> Change your username\n");
				strcat(output, "(DM)		    <username/id> <message> Send a direct message\n");
				strcat(output, "(ONLINE)		List people online\n");
				strcat(output, "(HELP)		  Show help menu\n");
				strcat(output, "(LEAVE)		  Leave the room\n");
				send_client(output, client);
			} else {
				send_client("(server): I have no clue what you are trying to do. :p\n", client);
			}
		} else { // Message
			sprintf(output, "(%s) %s\n", client->username, input);
			send_message(output, client);
		}
	}

	close(client->fd);
	sprintf(output, "(server): %s left the room\n", client->username);
	send_all(output);

	remove_client(client);
	printf("Client %d with address %s left\n", client->id, addr_to_string(client->addr));
	free(client);

	return NULL;
}

/**
 * Send a message to all clients but the provided one
 * @param s      The message to send
 * @param client The client to exclude
 */
void send_message(char *s, client_data *client){
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(CLIENTS[i]){
			if(CLIENTS[i]->id != client->id){
				write(CLIENTS[i]->fd, s, strlen(s));
			}
		}
	}
}

/**
 * Send a message to all clients
 * @param s The message to send
 */
void send_all(char *s) {
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(CLIENTS[i]){
			write(CLIENTS[i]->fd, s, strlen(s));
		}
	}
}

/**
 * Send a message to a specific client
 * @param s      The message to send
 * @param client The client to send to
 */
void send_client(char *s, client_data *client){
	write(client->fd, s, strlen(s));
}


/**
 * Lists all online clients
 * @param client The client to send the list to
 */
void list_online(client_data *client){
	int i;
	char s[64];
	for(i=0;i<MAX_CLIENTS;i++){
		if(CLIENTS[i]){
			send_client("Online Now:\n", client);
			send_client("------------\n", client);
			sprintf(s, "%d | %s\n", CLIENTS[i]->id, CLIENTS[i]->username);
			send_client(s, client);
		}
	}
}

/**
 * Strips newline and replaces with terminating null
 * @param s [description]
 */
void strip_newline(char *s){
	while(*s != '\0'){
		if(*s == '\n'){
			*s = '\0';
		}
		s++;
	}
}

/**
 * Converts sockaddr_in to a printable string
 * @param  addr A sockaddr_in instance
 * @return      The string address in IPv4 form
 */
char *addr_to_string(struct sockaddr_in addr) {
	char *buff = (char*)malloc(17 * sizeof(char));

	sprintf(buff, "%d.%d.%d.%d",
		addr.sin_addr.s_addr & 0xFF,
		(addr.sin_addr.s_addr & 0xFF00)>>8,
		(addr.sin_addr.s_addr & 0xFF0000)>>16,
		(addr.sin_addr.s_addr & 0xFF000000)>>24);

	return buff;
}
