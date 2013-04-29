/**
 * @File test_server.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements a test server that can connect to other servers and clients and sends out hard coded responses for testing.
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

 void test_hello_listen()
 {
    int listenfd = 0;
    int connfd = 0;

    int bytesRead = 0;
    char inputBuffer[512];
    char outputBuffer[512];
    memset(inputBuffer, '0', sizeof(inputBuffer)); 
    memset(outputBuffer, '0', sizeof(outputBuffer)); 

    listenfd = tcp_listen(4040);
    printf("Test Server listening on port 4040\n");

    while(1)
    {
      connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

      bytesRead = read(connfd, inputBuffer, sizeof(inputBuffer)-1);
      inputBuffer[bytesRead] = '\0';
      if(bytesRead < 0)
      {
        printf("Test Hello Read Error\n");
        return;
      }
      printf("Received Msg:\n");
      printf("%s\n", inputBuffer);

      strcpy(outputBuffer, "SERVER/HELLO\nSERVER/INFO&www.test_server.com:4000\nSERVICE/INFO&f,3,arr\nSERVICE/INFO&g,2,arr\n");

      write(connfd, outputBuffer, strlen(outputBuffer));

      close(connfd);
    }
 }

 int exchange_server_info(char* myHelloMsg, char* foreignServerName, int foreignServerPort)
{
  int sockfd;
  int size;
  struct sockaddr_in serv_addr;
  char inputBuff[512];
  memset(inputBuff, '\0', sizeof(inputBuff) );

  sockfd = tcp_connect(foreignServerName, foreignServerPort);
  
  write(sockfd, myHelloMsg, strlen(myHelloMsg));
  size = read( sockfd, inputBuff, (sizeof(char) * 1023) );
  // Don't close the socket because we assume the other server will close the socket when she is done sending her hello message to us.
  // I.E. we are acting as a client in this case.
  if( size > 0 )
  {
    printf("inputBuffer:/n");
    printf("%s\n", inputBuff);

    char* token;
    token = strtok(inputBuff, "\n");
    if(strstr(token,"SERVER/HELLO") == NULL)
    {
      printf("%s sent an invalid Server Announce message\n", foreignServerName);
      return 1;
    }
    // Skip the next line. It contains the server info, which we already know.
    token = strtok(NULL, "\n");

    // The rest of the lines are service info messages. Add these services to the services table   
    char tokSavePtr;
    char serviceName[255];
    char serviceReturnType[10];
    int serviceNumArgs;
    while(token != NULL)
    {
      memset(serviceName, '\0', sizeof(serviceName));
      memset(serviceReturnType, '\0', sizeof(serviceReturnType));
      serviceNumArgs = 0;
      token = strtok(NULL, "\n");
      if(token != NULL)
      {
        parse_service_info(token, serviceName, &serviceNumArgs, serviceReturnType);
      }
    }
    return 0;
  }
  return 1;
}

 void test_hello_send(char* serverName, int serverPort)
 {
    char outputBuffer[1024];
    printf("Sending Hello msg\n");
    strcpy(outputBuffer, "SERVER/HELLO\nSERVER/INFO&www.test_server.com:4000\nSERVICE/INFO&f,3,arr\nSERVICE/INFO&g,2,arr\n");
    exchange_server_info(outputBuffer, serverName, serverPort);
 }

 /**
 * Main
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{

  test_hello_listen();
  /*char serverName[255];
  int serverPort;

  strcpy(serverName, argv[1]);
  serverPort = atoi(argv[2]);

  test_hello_send(serverName, serverPort);*/
  return 0;
}
