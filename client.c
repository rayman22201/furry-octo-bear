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

linkedListStruct* serverList;
ServerNode* primaryServer;
int busyFlag = 0;
int unknownFlag = 0;

int send_client_request(char* requestString, ServerNode* curServer)
{
  int sockfd = tcp_connect(curServer->serverName, curServer->serverPort);
  write(sockfd, requestString, strlen(requestString));
  return sockfd;
}

void add_alts(char* serverResponse)
{
  char* token;
  ServerNode* newServer;

  //SERVER/MSG
  token = strtok(serverResponse, "\n");
  //SERVICE/INFO
  token = strtok(NULL, "\n");
  // The rest are server info strings
  while(token != NULL)
  {
    token = strtok(NULL, "\n");
    if(token != NULL)
    {
      newServer = malloc(sizeof(ServerNode));
      parse_server_info(token, newServer->serverName, &(newServer->serverPort));
      linkedList_addNode(serverList, newServer);
    }
  }
}

int parse_server_response(char* responseBuffer)
{
  if(strstr(responseBuffer, "SERVER/RESULT") != NULL)
  {
    busyFlag = 0;
    unknownFlag = 0;
    // Parse the Service Response
    if(strstr(responseBuffer, "SERVICE/MISMATCH") != NULL)
    {
      char* token;
      char* savePtr;
      char functionName[255];
      char returnType[10];
      int wrongNumArgs;
      int rightNumArgs;

      //SERVER/RESULT
      token = strtok(responseBuffer, "\n");
      //SERVICE/MISMATCH&num
      token = strtok(NULL, "\n");
      //SERVICE/MISMATCH
      strtok_r(token, "&", &savePtr);
      //num
      wrongNumArgs = atoi(strtok_r(NULL, "&", &savePtr));
      //SERVICE/INFO
      token = strtok(NULL, "\n");
      parse_service_info(token, functionName, &rightNumArgs, returnType);

      // Wrong number of args. print an error msg
      printf("Wrong number of arguments given.\n  You gave %d arguments, but %s requires %d arguments.\nPlease Try again.\n\n", wrongNumArgs, functionName, rightNumArgs);
      return 0;
    }
    else if(strstr(responseBuffer, "SERVICE/ERROR") != NULL)
    {
      // Something went wrong at the service level
      printf("There was an error executing the function.\n");
      return 0;
    }
    else if(strstr(responseBuffer, "SERVICE/RESULT") != NULL)
    {
      // Yay, we got a good result. Print the answer.
      char* token;
      char* savePtr;
      char resultType[10];

      //SERVER/RESULT
      token = strtok(responseBuffer, "\n");
      //SERVICE/RESULT&type&numResults&results
      token = strtok(NULL, "\n");
      //SERVICE/RESULT
      strtok_r(token, "&", &savePtr);
      strcpy(resultType, strtok_r(NULL, "&", &savePtr));
      if(strstr(resultType, "arr") != NULL)
      {
        // We have an array of ints;
        int numResults;
        int results[100];
        numResults = atoi(strtok_r(NULL, "&", &savePtr));
        printf("Server returned an array of %d integers:\n", numResults);

        int i;
        for(i = 0; i < numResults; i++)
        {
          results[i] = atoi(strtok_r(NULL, ",", &savePtr));
          printf("  result[%d] : %d\n", i, results[i]);
        }
        printf("\n");
        return 1;
      }
      else
      {
        printf("Sorry, This client does not support results of type %s\n\n", resultType);
        return 0;
      }
    }
    else
    {
      printf("Invalid Service response.\n");
      return 0;
    }
  }
  else if(strstr(responseBuffer, "SERVER/BUSY") != NULL)
  {
    busyFlag = 1;
    // if we have any alts, add them to the server list
    if(strstr(responseBuffer, "SERVER/INFO&none") == NULL)
    {
      add_alts(responseBuffer);
    }
    return 0;
  }
  else if(strstr(responseBuffer, "SERVER/UNKNOWN") != NULL)
  {
    unknownFlag = 1;
    // if we have any alts, add them to the server list
    if(strstr(responseBuffer, "SERVER/INFO&none") == NULL)
    {
      add_alts(responseBuffer);
    }
    return 0;
  }
  else if(strstr(responseBuffer, "SERVER/ERROR") != NULL)
  {
    // Some misc server error
    parse_server_error(responseBuffer);
    return 0;
  }
  else
  {
    printf("Invalid Server Response.\n");
    return 0;
  }
  return 0;
}

void build_request_string(char* outputBuffer, char* functionName, int argc, const int* argv)
{
  sprintf(outputBuffer, "CLIENT/REQUEST&%s", functionName);
  int i = 0;
  char argStr[10];
  for(i = 0; i < argc; i++)
  {
    sprintf(argStr, ",%d", argv[i]);
    strcat(outputBuffer, argStr);
  }
  strcat(outputBuffer, "\n");
}

int execute_server_request(char* requestString, ServerNode* curServer)
{
  char responseBuffer[1024];
  memset(responseBuffer, '\0', sizeof(responseBuffer));
  int responsefd = send_client_request(requestString, curServer);
  int size = read( responsefd, responseBuffer, sizeof(responseBuffer) );
  if(size > 0)
  {
    return parse_server_response(responseBuffer);
  }
  printf("Server Responded with no data.\n");
  return 0;
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

  char serverString[255];
  primaryServer = malloc(sizeof(ServerNode));

  serverList = linkedList_newList();
  linkedList_addNode(serverList, primaryServer);
  ServerNode* curServer;
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

    build_request_string(outputBuffer, functionName, functionArgc, functionArgs);

    // For each server in the server list, send a request and process the result
    unknownFlag = 0;
    busyFlag = 0;
    linkedList_reset_iterator(serverList);
    curServer = linkedList_foreach(serverList);
    while(curServer != NULL)
    {
      execute_server_request(outputBuffer, curServer);
      curServer = linkedList_foreach(serverList); 
    }
    if(busyFlag == 1)
    {
      printf("All Known Servers are Busy Right Now. :-(\n");
    }
    else if(unknownFlag == 1)
    {
      printf("Unable to find any Server that knows how to execute %s with %d arguments\n", functionName, functionArgc);
    }
    linkedList_freeAllButFirst(serverList);
  }  
}
