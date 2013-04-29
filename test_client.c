/**
 * @File test_client.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements a test client that can connect to servers and sends out hard coded responses for testing.
 */

//std lib stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//stuff to browse the local filesystem
#include <sys/types.h>
#include <dirent.h>

//parsing library
#include "protocol_parser.c"

//networking library
#include "network_library.c"

  /**
 * Main
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{
  char outputBuffer[512];
  char inputBuffer[1024];
  char serverName[255];
  int serverPort;

  strcpy(serverName, argv[1]);
  serverPort = atoi(argv[2]);

  printf("requesting f(4,5,6) from %s:%d\n", serverName, serverPort);
  strcpy(outputBuffer, "CLIENT/REQUEST&f,4,5,6\n");

  int sockfd = tcp_connect(serverName, serverPort);
  write(sockfd, outputBuffer, strlen(outputBuffer));

  int size = read( sockfd, inputBuffer, sizeof(inputBuffer) );
  if(size > 0)
  {
    inputBuffer[size] = '\0';
    printf("server Response:\n");
    printf("%s\n", inputBuffer);
  }
  close(sockfd);

  return 0;
}
