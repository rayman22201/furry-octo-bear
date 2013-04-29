/**
 * @File client.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements the Client described in the assignment
 *
 * Data Structures I need:
 *   servers list = linked list (maybe with lazy delete if I want to be fancy)
 */

//std lib stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//stuff to browse the local filesystem
#include <sys/types.h>
#include <dirent.h>

//fancy data structures
#include "linked_list.c"

//parsing library
#include "protocol_parser.c"

//networking library
#include "network_library.c"

typedef struct
{
  char serverName[255];
  int serverPort; // In Network Byte Order.
} ServerNode;

int send_client_request(char* requestString, ServerNode* curServer)
{
  int sockfd = tcp_connect(curServer->serverName, curServer->serverPort);
  write(sockfd, requestString, strlen(requestString));
  return sockfd;
}

linkedListStruct* parse_alts(char* altServerString)
{
  linkedListStruct* serverList;
  serverList = linkedList_newList();

  //parse the servers into a linkedlist of ServerNodes

  return serverList;
}

void try_alt(char* responseBuffer, linkedListStruct* altList)
{

}

void parse_server_response(char* responseBuffer)
{
  if(strstr(responseBuffer, "SERVER/RESULT") != NULL)
  {
    // Parse the Service Response
    if(strstr(responseBuffer, "SERVICE/MISMATCH") != NULL)
    {
      // Wrong number of args. print an error msg
    }
    else if(strstr(responseBuffer, "SERVICE/ERROR") != NULL)
    {
      // Something went wrong at the service level
    }
    else if(strstr(responseBuffer, "SERVICE/RESULT") != NULL)
    {
      // Yay, we got a good result. Print the answer.
    }
    else
    {
      printf("Invalid Service response.\n");
      return;
    }
  }
  else if(strstr(responseBuffer, "SERVER/BUSY") != NULL)
  {
    // Busy Server, either try the alts or print an error.
  }
  else if(strstr(responseBuffer, "SERVER/UNKNOWN") != NULL)
  {
    // Unknown function. Either try alts or print an error.
  }
  else if(strstr(responseBuffer, "SERVER/ERROR") != NULL)
  {
    // Some misc server error
  }
  else
  {
    printf("Invalid Server Response.\n");
    return;
  }
}

/**
 * Main
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{
  if(argc < 2)
  {
    printf("Please enter the name of a primary server.\n");
    return 1;
  }
  char inputBuffer[1024];
  char outputBuffer[1024];
  char responseBuffer[1024];

  char serverString[255];
  ServerNode* primaryServer;
  primaryServer = malloc(sizeof(ServerNode));

  char functionName[255];
  int functionArgc;
  int functionArgs[100];

  strcpy(serverString, argv[1]);
  parse_host_string(serverString, primaryServer->serverName, &(primaryServer->serverPort));
  printf("Using %s as Primary Server on Port: %d\n", primaryServer->serverName, primaryServer->serverPort);

  while(1)
  {
    memset(inputBuffer, '\0', sizeof(inputBuffer));
    memset(outputBuffer, '\0', sizeof(outputBuffer));
    memset(responseBuffer, '\0', sizeof(responseBuffer));
    functionArgc = 0;

    printf("Enter a function to process. Type 'help' for usage information:\n");
    if(gets(inputBuffer) == NULL)
    {
      printf("Error reading input.\n");
      return 1;
    }
    if(strstr(inputBuffer, "quit") != NULL)
    {
      return 0;
    }
    if(strstr(inputBuffer, "help") != NULL)
    {
      printf("\nEnter a function to request and then hit enter OR type 'quit' to exit the program.\n  The function format is as follows: name(arg1,arg2,...,argN)\n  Example: f(3,5,4)\n\n");
      continue;
    }
    parse_client_input(inputBuffer, functionName, &functionArgc, functionArgs);

    sprintf(outputBuffer, "CLIENT/REQUEST&%s", functionName);
    int i = 0;
    char argStr[10];
    for(i = 0; i < functionArgc; i++)
    {
      sprintf(argStr, ",%d", functionArgs[i]);
      strcat(outputBuffer, argStr);
    }
    strcat(outputBuffer, "\n");
    printf("Request String:\n%s", outputBuffer);

    int responsefd = send_client_request(outputBuffer, primaryServer);
    int size = read( responsefd, responseBuffer, sizeof(responseBuffer) );
    if(size > 0)
    {
      printf("Server Response:\n%s\n", responseBuffer);
      parse_server_response(responseBuffer);
    }
  }
  //get the name and port of my primary server from the command line
  //Add primary server to the server list
  //Set Unknown and Busy flags to false
  //Set exit flag to false.
  //
  //While exit flag is false
  //  Prompt the user for input: i.e. a function to compute with parameters: f(3,2)
  //  if user enters exit command
  //    set exit flag to true
  //    break
  //
  //  For each server in server list
  //   send request
  //   if response is the answer
  //    set unknown and busy flags to false
  //    display the answer
  //    break
  //   if response is unknown
  //    set unknown flag to true.
  //    add alternate servers to server list
  //   if response is busy signal
  //    set busy flag to true.
  //    add alternate servers to server list
  //  End For
  //  
  //  if busy flag is true 
  //    display error message
  //  if unknown flag is true
  //    display error message
  //  set unknown and busy flags to false
  //  Remove all but first server from the server list
  //End While  
}