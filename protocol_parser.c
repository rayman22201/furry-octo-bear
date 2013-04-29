/**
 * @File protocol_parser.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements functions that parse protocol strings into useful variables
 */

//std lib stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// So my syntax checker doesn't bother me with false positives
// strtok_r is the reentrant version of strtok, this way we don't mess up any strtok that is going on in the calling function.
extern char *strtok_r(char *str, const char *delim, char **saveptr);

void parse_service_info(char* buffer, char* name, int* numArgs, char* returnType)
{
  char* savePtr;
  char myBuffer[512];
  char* header;
  char* strNumArgs;
  memset(myBuffer, '\0', sizeof(myBuffer));
  strcpy(myBuffer, buffer); //don't mess with the original buffer
  header = strtok_r(myBuffer, "&,\n", &savePtr);
  if(strstr(header, "SERVICE/INFO") == NULL)
  {
    printf("Not a Valid Service Info String.\n");
    exit(0);
  }
  strcpy(name, strtok_r(NULL, "&,\n", &savePtr));
  strNumArgs = strtok_r(NULL, "&,\n", &savePtr);
  (*numArgs) = atoi(strNumArgs);
  strcpy(returnType,strtok_r(NULL, "&,\n", &savePtr));
}

void parse_host_string(char* buffer, char* name, int* portNum)
{
  char* savePtr;
  char myBuffer[512];
  strcpy(myBuffer, buffer);
  strcpy(name, strtok_r(myBuffer, ":", &savePtr));
  (*portNum) = atoi(strtok_r(NULL, ":", &savePtr));
}

void parse_host_info(char* buffer, char* name, int* portNum)
{
  char* savePtr;
  char* token;
  char myBuffer[512];
  strcpy(myBuffer, buffer);
  token = strtok_r(myBuffer, "&", &savePtr);
  if(strstr(token, "SERVER/INFO") == NULL)
  {
    printf("Not a Valid Server Announce String.\n");
    exit(1);
  }
  
  token = strtok_r(NULL, "&", &savePtr);
  parse_host_string(token, name, portNum);
}

int parse_server_info(char* buffer, char* name, int* portNum)
{
  char* savePtr;
  char myBuffer[512];
  char* header;
  char* strPortNum;

  strcpy(myBuffer, buffer);
  header = strtok_r(myBuffer, "&:\n", &savePtr);
  if(strstr(header, "SERVER/INFO") == NULL)
  {
    printf("Not a Valid Server Announce String.\n");
    exit(0);
  }
  char* info;
  info = strtok_r(NULL, "&:\n", &savePtr);
  if(strstr(info, "name") != NULL)
  {
    //No server info available
    return 1;
  }

  strcpy(name, info);
  strPortNum = strtok_r(NULL, "&:\n", &savePtr);
  (*portNum) = atoi(strPortNum);
  return 0;
}

void parse_server_mismatch(char* buffer, char* serviceName, int* rightNumArgs, int* userNumArgs)
{
  char* savePtr;
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok_r(myBuffer, "&,\n", &savePtr);
  if(strstr(header, "SERVER/MISMATCH") == NULL)
  {
    printf("Not a Valid Server Mismatch String.\n");
    exit(0);
  }
  strcpy(serviceName, strtok_r(NULL, "&,\n", &savePtr));
  (*rightNumArgs) = atoi(strtok_r(NULL, "&,\n", &savePtr));
  (*userNumArgs) = atoi(strtok_r(NULL, "&,\n", &savePtr));
}

void parse_server_error(char* buffer)
{
  char* savePtr;
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok_r(myBuffer, "&,\n", &savePtr);
  if(strstr(header, "SERVER/ERROR") == NULL)
  {
    printf("Not a Valid Server ERROR String.\n");
    exit(0);
  }
  printf("Server Encountered an Error\n");
  char* msg = strtok_r(NULL, "&,\n", &savePtr);
  if(msg != NULL)
  {
    printf("  %s\n", msg);
  }
}

void parse_server_result(char* buffer, char* resultType, int* resultLen, void** resultBuffer)
{
  char* savePtr;
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok_r(myBuffer, "&,\n", &savePtr);
  if(strstr(header, "SERVER/RESULT") == NULL)
  {
    printf("Not a Valid Server Result String.\n");
    exit(0);
  }
  strcpy(resultType, strtok_r(NULL, "&,\n", &savePtr));
  (*resultLen) = atoi(strtok_r(NULL, "&,\n", &savePtr));
  if(strstr(resultType, "arr") != NULL)
  {
    //an array of integers
    int* results;
    results = malloc( (sizeof(int) * (*resultLen)) );
    int i;
    for(i = 0; i < (*resultLen); i++)
    {
      results[i] = atoi(strtok_r(NULL, "&,\n", &savePtr));
    }
    (*resultBuffer) = results; 
  }
  else if (strstr(resultType, "raw") != NULL)
  {
    //raw data
    char* results;
    results = malloc((*resultLen));
    memcpy( results, strtok_r(NULL, "&,\n", &savePtr), (*resultLen) );
  }
  else
  {
    printf("Unsupported Result Type");
    exit(0);
  }
}

void parse_client_request(char* buffer, char* serviceName, int* argc, int* argv)
{
  char* savePtr;
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok_r(myBuffer, "&,\n", &savePtr);
  if(strstr(header, "CLIENT/REQUEST") == NULL)
  {
    printf("Not a Valid Client Request String.\n");
    exit(0);
  }
  char* token;
  token = strtok_r(NULL, "&,\n", &savePtr);
  strcpy(serviceName, token);

  (*argc) = 0;
  while(token != NULL)
  {
    token = strtok_r(NULL, "&,\n", &savePtr);
    argv[(*argc)] = atoi(token);
    (*argc)++;
  }
}
