#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef SUPPORT_HEADER
#define SUPPORT_HEADER
#define MAX_CLIENTS	128

static unsigned int CLIENT_COUNT = 0;
static unsigned int ID_DISPATCH = 1;

/* Client structure */
typedef struct {
	struct sockaddr_in addr;
	int fd;
	int id;
	char username[32];
} client_data;

client_data *CLIENTS[MAX_CLIENTS];

// Client Methods
void add_client(client_data *client);
void remove_client(client_data *client);
void *handle_client(client_data *client);

// Message Methods
void send_message(char *s, client_data *client);
void send_all(char *s);
void send_client(char *s, client_data *client);

// Utility Methods
void list_online(client_data *client);
void strip_newline(char *s);
char *addr_to_string(struct sockaddr_in addr);

#endif
