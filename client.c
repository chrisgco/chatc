#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
  char* args[4];

  if (argc < 3) {
     fprintf(stderr,"usage %s host port\n", argv[0]);
     exit(0);
  }

  args[0] = "nc";
  args[1] = argv[1];
  args[2] = argv[2];
  args[3] = NULL;

  printf("Command List:\n");
  printf("--------------\n");
  printf("(USERNAME)	<username> Change your username\n");
  printf("(DM)		    <reference> <message> Send a direct message\n");
  printf("(ONLINE)		List people onlineS\n");
  printf("(HELP)		  Show help menu\n");
  printf("(LEAVE)		  Leave the room\n\n");

  execvp(args[0], args);

  return 0;
}
