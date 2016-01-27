#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/*
clang client.c 104.129.86.9314 - Connect to specific server

login or signup: login

username: iieqwuigfeiwq
password: hwqihiqhii

connected

failed to connect

conversations:
- @john john smith
- @kyle kyle roberts

who do you want to talk to (@username): @kyle

Message History with Kyle Roberts (@kyle):

Kyle (Dec, 5, 2015 at 10:01pm): hewifh ewhfoewqhfewh
You (Dec 5, 2015 at 10:02pm): hhefhew ehwifeho heiwoq

Compose: hey what's up

You (Dec 6, 2015 at 4:59pm): hey what's up

Compose:

Kyle (Dec 6, 2015 at 5:00pm): Hey

Compose:





 */

int main(int argc, char **argv) {


  if( argc == 2 ) {
     printf("The argument supplied is %s\n", argv[1]);
  }
  else if( argc > 2 ) {
     printf("Too many arguments supplied.\n");
  }
  else {
     printf("One argument expected.\n");
  }

  int socket_id;
  char buffer[256];
  int i;

  //create the socket
  socket_id = socket( AF_INET, SOCK_STREAM, 0 );

  //bind to port/address
  struct sockaddr_in sock;
  sock.sin_family = AF_INET;
  sock.sin_port = htons(24601);
  //Set the IP address to connect to
  //127.0.0.1 is the "loopback" address of any machine
  inet_aton( "127.0.0.1", &(sock.sin_addr) );
  bind( socket_id, (struct sockaddr *)&sock, sizeof(sock));

  //attempt a connection
  i = connect(socket_id, (struct sockaddr *)&sock, sizeof(sock));
  printf("<client> connect returned: %d\n", i);

  read( socket_id, buffer, sizeof(buffer));
  printf("<client> received: [%s]\n", buffer );

  return 0;
}
