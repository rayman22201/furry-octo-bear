/**
 * @File service_g.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements a service for use with an assignment 2 server.
 * Returns the Fibonacci sequence between the range of the two input values
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
  char* functionName;
  functionName = parse_function_name(argv[0]);
  
  char returnType[] = "arr";
  int numArgs = 2;

  // No args.
  if( argc < 2 ) {
    //get the function name from the filepath
    printf("SERVICE/INFO&%s,%d,%s\n", functionName, numArgs, returnType);
    return 0;
  }
  else if( argc != (numArgs+1))
  {
    printf("SERVICE/MISMATCH&%d\nSERVICE/INFO&%s,%d,%s\n", (argc - 1), functionName, numArgs, returnType);
    return 0;
  }
  else
  {
    char outputBuffer[1024];
    memset(outputBuffer, '\0', sizeof(outputBuffer));
    char resultHeader[255];
    char curVal[10];
    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int range = end - start;
    int curFib = 0;
    int prevFib = 0;
    int prevFib2 = 0;

    int i = 0;
    int fibCount = 0;
    while(curFib < end)
    {
      if(i < 2)
      {
        prevFib2 = prevFib;
        prevFib = curFib;
        curFib = i;
      }
      else
      {
        prevFib2 = prevFib;
        prevFib = curFib;
        curFib = prevFib + prevFib2;
      }
      if( (curFib > start) && (curFib < end) )
      {
        sprintf(curVal, "%d,", curFib);
        strcat(outputBuffer, curVal);
        fibCount++;
      }
      i++;
    }
    outputBuffer[ (strlen(outputBuffer)-1) ] = '\0';
    sprintf(resultHeader, "SERVICE/RESULT&arr&%d&", fibCount);
    printf("%s%s\n", resultHeader, outputBuffer);
    return 0;
  }
}
