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

void parse_service_info(char* buffer, char* name, int* numArgs, char* returnType)
{
  char myBuffer[512];
  char* header;
  char* strNumArgs;

  strcpy(myBuffer, buffer); //don't mess with the original buffer
  header = strtok(myBuffer, "&,\n");
  if(strstr(header, "SERVICE/INFO") == NULL)
  {
    printf("Not a Valid Service Info String.");
    exit(0);
  }
  strcpy(name, strtok(NULL, "&,\n"));
  strNumArgs = strtok(NULL, "&,\n");
  (*numArgs) = atoi(strNumArgs);
  strcpy(returnType,strtok(NULL, "&,\n"));
}

void parse_host_string(char* buffer, char* name, int* portNum)
{
  strcpy(name, strtok(buffer, ":"));
  (*portNum) = atoi(strtok(NULL, ":"));
}

int parse_server_info(char* buffer, char* name, int* portNum)
{
  char myBuffer[512];
  char* header;
  char* strPortNum;

  strcpy(myBuffer, buffer);
  header = strtok(myBuffer, "&:\n");
  if(strstr(header, "SERVER/INFO") == NULL)
  {
    printf("Not a Valid Server Announce String.");
    exit(0);
  }
  char* info;
  info = strtok(NULL, "&:\n");
  if(strstr(info, "name") != NULL)
  {
    //No server info available
    return 1;
  }

  strcpy(name, info);
  strPortNum = strtok(NULL, "&:\n");
  (*portNum) = atoi(strPortNum);
  return 0;
}

void parse_server_mismatch(char* buffer, char* serviceName, int* rightNumArgs, int* userNumArgs)
{
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok(myBuffer, "&,\n");
  if(strstr(header, "SERVER/MISMATCH") == NULL)
  {
    printf("Not a Valid Server Mismatch String.");
    exit(0);
  }
  strcpy(serviceName, strtok(NULL, "&,\n"));
  (*rightNumArgs) = atoi(strtok(NULL, "&,\n"));
  (*userNumArgs) = atoi(strtok(NULL, "&,\n"));
}

void parse_server_error(char* buffer)
{
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok(myBuffer, "&,\n");
  if(strstr(header, "SERVER/ERROR") == NULL)
  {
    printf("Not a Valid Server ERROR String.");
    exit(0);
  }
  printf("Server Encountered an Error\n");
  char* msg = strtok(NULL, "&,\n");
  if(msg != NULL)
  {
    printf("  %s\n", msg);
  }
}

void parse_server_result(char* buffer, char* resultType, int* resultLen, void** resultBuffer)
{
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok(myBuffer, "&,\n");
  if(strstr(header, "SERVER/RESULT") == NULL)
  {
    printf("Not a Valid Server Result String.");
    exit(0);
  }
  strcpy(resultType, strtok(NULL, "&,\n"));
  (*resultLen) = atoi(strtok(NULL, "&,\n"));
  if(strstr(resultType, "arr") != NULL)
  {
    //an array of integers
    int* results;
    results = malloc( (sizeof(int) * (*resultLen)) );
    int i;
    for(i = 0; i < (*resultLen); i++)
    {
      results[i] = atoi(strtok(NULL, "&,\n"));
    }
    (*resultBuffer) = results; 
  }
  else if (strstr(resultType, "raw") != NULL)
  {
    //raw data
    char* results;
    results = malloc((*resultLen));
    memcpy( results, strtok(NULL, "&,\n"), (*resultLen) );
  }
  else
  {
    printf("Unsupported Result Type");
    exit(0);
  }
}

void parse_client_request(char* buffer, char* serviceName, int* argc, int* argv)
{
  char myBuffer[512];
  char* header;

  strcpy(myBuffer, buffer);
  header = strtok(myBuffer, "&,\n");
  if(strstr(header, "CLIENT/REQUEST") == NULL)
  {
    printf("Not a Valid Client Request String.");
    exit(0);
  }
  char* token;
  token = strtok(NULL, "&,\n");
  strcpy(serviceName, token);

  (*argc) = 0;
  while(token != NULL)
  {
    token = strtok(NULL, "&,\n");
    argv[(*argc)] = atoi(token);
    (*argc)++;
  }
}
