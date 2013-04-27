/**
 * @File service_f.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements a service for use with an assignment 2 server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* parse_function_name(const char* path)
{
  char filepath[1024];
  strcpy(filepath, path);
  char* token;
  char* functionName;
  token = strtok(filepath, "/");
  token = strtok(NULL, "/"); //skip the first token because it is always just "." (the current directory)
  while(token != NULL)
  {
    if(strstr(token, ".") != NULL)
    {
      token = strtok(token, ".");
      functionName = token;
      return token;
    }
    token = strtok(NULL, "/");
  }
  return NULL;
}

/**
 * Main
 * Calling with no arguments should return a service description string that the server can parse and use to advertise the service.
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{
  // No args.
  if( argc < 2 ) {
    //get the function name from the filepath
    char* functionName;
    functionName = parse_function_name(argv[0]);
    printf("SERVICE/INFO&%s,3,arr\n", functionName);
    return 0;
  }
  else if( argc != 4)
  {
    printf("SERVICE/ERROR&Argument Mismatch\n");
    return 0;
  }
  else
  {
    printf("42\n");
    return 0;
  }
}
