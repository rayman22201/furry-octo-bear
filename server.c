/**
 * @File server.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements the Server described in the assignment
 * The actual services offered are based on external programs through a plugin architecture.
 * @See services/service_f.c
 *
 * Data Structures I need:
 *   services table = hash table --> keyed on function name+signature
 *   service node = struct describing the service
 */

//std lib stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//stuff to browse the local filesystem
#include <sys/types.h>
#include <dirent.h>

//fancy custom data-structures
#include "hash_table.c"
#include "linked_list.c"

//parsing library
#include "protocol_parser.c"

//networking library
#include "network_library.c"

typedef struct
{
  char serviceName[255];
  char serverName[255];
  char returnType[10];
  char signature[1024];
  int numArgs;
  int serverPort; // In Network Byte Order. 
} ServiceNode;

hashTable* servicesTable;
linkedListStruct* myServices;
char myHelloMsg[2048];
char myHostName[255];
int myPort;

ServiceNode* register_service(char* name, int numArgs, char* returnType, char* serverName, int port)
{
  ServiceNode* newService;
  newService = malloc(sizeof(ServiceNode));

  strcpy(newService->serviceName, name);
  strcpy(newService->serverName, serverName);
  strcpy(newService->returnType, returnType);

  newService->numArgs = numArgs;
  newService->serverPort = port;

  sprintf(newService->signature, "%s,%d,%s", name, numArgs, returnType);
  printf("  ServiceSignature: %s\n", newService->signature);
  hashTable_addElement(servicesTable, newService->signature, (void*)newService);
  return newService;
}

void register_local_service(char* filepath)
{
  char descriptionBuffer[1024];
  char serviceName[255];
  char returnType[10];
  int numArgs = -1;
  char serverName[] = "localhost";
  int serverPort = 0;

  //run the service with no args to get it's info
  FILE* fp;
  fp = popen(filepath, "r");
  if(fp == NULL)
  {
    perror("Unable to run local Service");
    exit(1);
  }
  // Parse the info
  fgets(descriptionBuffer, sizeof(descriptionBuffer), fp);
  
  // Copy the string to myServices
  char* serviceString;
  serviceString = malloc(sizeof(char) * strlen(descriptionBuffer));
  strcpy(serviceString, descriptionBuffer);
  linkedList_addNode(myServices, serviceString);

  // Parse the string and register the service
  parse_service_info(descriptionBuffer, serviceName, &numArgs, returnType);
  printf("Local Service Detected:\n");
  printf("  Name: %s, numArgs: %d, returnType: %s\n", serviceName, numArgs, returnType);
  
  // Add the service to the table
  register_service(serviceName, numArgs, returnType, serverName, serverPort);
}

void find_local_services()
{
  DIR *dirPtr;
  struct dirent *curFile;
  dirPtr = opendir ("./services");

  if (dirPtr != NULL)
  {
    curFile = readdir(dirPtr);
    while (curFile != NULL)
    {
      if( strstr(curFile->d_name, ".service") != NULL )
      {
        char filepath[1024];
        sprintf(filepath, "./services/%s",curFile->d_name);
        register_local_service(filepath);
      }
      curFile = readdir(dirPtr);
    }
    (void) closedir (dirPtr);
  }
  else
  {
    perror ("Couldn't open the directory");
  }
}

int exchange_server_info(char* foreignServerName, int foreignServerPort)
{
  int sockfd;
  char recvBuff[1024];
  int size;
  struct sockaddr_in serv_addr; 
  memset(recvBuff, '0',sizeof(recvBuff));

  sockfd = tcp_connect(foreignServerName, foreignServerPort);
  
  write(sockfd, myHelloMsg, strlen(myHelloMsg));
  size = read( sockfd, recvBuff, (sizeof(recvBuff)-1) );
  // Don't close the socket because we assume the other server will close the socket when she is done sending her hello message to us.
  // I.E. we are acting as a client in this case.
  if( size > 0 )
  {
    char* token;
    token = strtok(recvBuff, "\n");
    if(strstr(token,"SERVER/HELLO") == NULL)
    {
      printf("%s sent an invalid Server Announce message\n", foreignServerName);
      return 1;
    }
    // Skip the next line. It contains the server info, which we already know.
    token = strtok(NULL, "\n");

    // The rest of the lines are service info messages. Add these services to the services table   
    while(token != NULL)
    {
      token = strtok(NULL, "\n");
      char serviceName[255];
      char serviceReturnType[10];
      int serviceNumArgs;
      parse_service_info(token, serviceName, &serviceNumArgs, serviceReturnType);
      register_service(serviceName, serviceNumArgs, serviceReturnType, foreignServerName, foreignServerPort);
    }
    return 0;
  }
  return 1;
}

/**
 * Main
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{
  //initialize the services table
  servicesTable = hashTable_create(50, &linkedList_free_function, &linkedList_init_function, &linkedList_add_function, &linkedList_remove_function);
  //initalize a list of my local services
  myServices = linkedList_newList();

  // Find all my own services and register them in my services table
  find_local_services();

  // Read the port I am supposed to use off the command line
  if(strstr(argv[1], ":") == NULL)
  {
    // No Host name was supplied by the user. Attempt to figure out my Network Host name
    getdomainname(myHostName, 255);
    if(strstr(myHostName, "(none)") != NULL)
    {
      gethostname(myHostName, 255);
    }

    myPort = atoi(argv[1]);
  }
  else
  {
    // Use the network Host name provided by the argument
    char localServerStr[255];
    strcpy(localServerStr, argv[1]);
    parse_host_string(localServerStr, myHostName, &myPort);
  }

  //generate my hello message 
  sprintf(myHelloMsg, "SERVER/HELLO\nSERVER/INFO&%s:%d\n", myHostName, myPort);
  char* serviceString;
  linkedList_reset_iterator(myServices);
  serviceString = linkedList_foreach(myServices);
  while(serviceString != NULL)
  {
    strcat(myHelloMsg, serviceString);
    serviceString = linkedList_foreach(myServices);
  }

  // Read a configuration of other known servers
  //  For each server
  //    Send a Hello Message
  //  End For
  int i;
  for(i = 2; i < argc; i++)
  {
    //parse the server info;
    char curServerString[255];
    char* curServerName;
    int curServerPort;
    strcpy(curServerString, argv[i]);
    parse_host_string(curServerString, curServerName, &curServerPort);

    //Introduce myself to the other servers
    //@TODO: If I make linked list thread safe, then I can do this in a multithreaded fashion.
    exchange_server_info(curServerName, curServerPort);
  }

  // Accept Requests Forever
  //  if msg is from a Server
  //    Add the info to my services table
  //  if msg is from a Client
  //    attempt to process the request
  // End While
}
