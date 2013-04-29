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

  sprintf(newService->signature, "/%s/", name);
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
  int serverPort = myPort;

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
  
  // Add the service to the table
  register_service(serviceName, numArgs, returnType, serverName, serverPort);
}

void find_local_services()
{
  DIR *dirPtr;
  struct dirent *curFile;
  dirPtr = opendir ("./services");

  printf("Local Services Detected from ./services directory:\n");
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

int add_server_info(char* buffer)
{
  char inputBuff[1024];
  char foreignServerName[255];
  int foreignServerPort;
  char* token;

  strcpy(inputBuff, buffer);
  token = strtok(inputBuff, "\n");
  if(strstr(token,"SERVER/HELLO") == NULL)
  {
    printf("Server sent an invalid Server Announce message\n");
    return 1;
  }
  // The next line contains the server info.
  token = strtok(NULL, "\n");
  // Parse out the server info
  parse_host_info(token, foreignServerName, &foreignServerPort);

  // The rest of the lines are service info messages. Add these services to the services table   
  char tokSavePtr;
  char serviceName[255];
  char serviceReturnType[10];
  int serviceNumArgs;

  printf("Foreign Services Detected from %s:%d:\n", foreignServerName, foreignServerPort);
  while(token != NULL)
  {
    memset(serviceName, '\0', sizeof(serviceName));
    memset(serviceReturnType, '\0', sizeof(serviceReturnType));
    serviceNumArgs = 0;
    token = strtok(NULL, "\n");
    if(token != NULL)
    {
      parse_service_info(token, serviceName, &serviceNumArgs, serviceReturnType);
      register_service(serviceName, serviceNumArgs, serviceReturnType, foreignServerName, foreignServerPort);
    }
  }
  return 0;
}

int exchange_server_info(char* foreignServerName, int foreignServerPort)
{
  int sockfd;
  int size;
  struct sockaddr_in serv_addr;
  char inputBuff[1024];
  memset(inputBuff, '\0', sizeof(inputBuff) );

  sockfd = tcp_connect(foreignServerName, foreignServerPort);
  
  write(sockfd, myHelloMsg, strlen(myHelloMsg));
  size = read( sockfd, inputBuff, sizeof(inputBuff) );
  close(sockfd);
  if( size > 0 )
  {
    add_server_info(inputBuff);
    return 0;
  }
  return 1;
}

int am_I_busy()
{
  int chance = rand() % 100; 
  if(chance < 50)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

void print_alt_servers(char* outputBuffer, linkedListStruct* serviceList)
{
  ServiceNode* curNode;
  char curAltServer[255];

  linkedList_reset_iterator(serviceList);
  curNode = linkedList_foreach(serviceList);
  while(curNode != NULL)
  {
    if(strstr(curNode->serverName, "localhost") == NULL)
    {
      sprintf(curAltServer, "SERVER/INFO%s&:%d\n", curNode->serverName, curNode->serverPort);
      strcat(outputBuffer, curAltServer);
    }
    curNode = linkedList_foreach(serviceList);
  }
}

int run_local_service(char* outputBuffer, ServiceNode* curService, int argc, const int* argv)
{
  char command[1024];

  sprintf(command, "./services/%s.service ", curService->serviceName);
  int i;
  char strArg[10];
  for(i = 0; i < argc; i++)
  {
    sprintf(strArg, "%d ", argv[i]);
    strcat(command, strArg);
  }

  FILE* fp;
  fp = popen(command, "r");
  if(fp == NULL)
  {
    perror("Unable to run local Service");
    exit(1);
  }
  char commandBuffer[2048];
  fread(commandBuffer, sizeof(char), (sizeof(commandBuffer) - 1), fp);
  sprintf(outputBuffer, "SERVER/RESULT\n");
  strcat(outputBuffer, commandBuffer);

  return 1;
}

int run_service(char* outputBuffer, char* functionName, int argc, const int* argv)
{
  char key[255];
  linkedListStruct* serviceList;

  sprintf(key, "/%s/", functionName);
  serviceList = (linkedListStruct*)hashTable_lookup(servicesTable, key);

  if(linkedList_isEmptyList(serviceList))
  {
    sprintf(outputBuffer, "SERVER/UNKNOWN\nSERVICE/INFO&%s,%d\nSERVER/INFO&none", functionName, argc);
    return 0;
  }
  else
  {
    linkedList_reset_iterator(serviceList);
    ServiceNode* curNode;
    curNode = linkedList_foreach(serviceList);
    if( strstr(curNode->serverName, "localhost") != NULL )
    {
      // We have a copy of this service locally.
      // are we busy?
      if(!am_I_busy())
      {
        // Run the service and return the result
        run_local_service(outputBuffer, curNode, argc, argv);
        return 1;
      }
      else
      {
        // I'm busy return some alternates
        ServiceNode* nextNode = linkedList_foreach(serviceList);
        if(nextNode == NULL)
        {
          // There are not alternates. Tough luck
          sprintf(outputBuffer, "SERVER/BUSY\nSERVICE/INFO&%s,%d,%s\nSERVER/INFO&none\n", curNode->serviceName, curNode->numArgs, curNode->returnType);
        }
        else
        {
          sprintf(outputBuffer, "SERVER/BUSY\nSERVICE/INFO&%s,%d,%s\n", curNode->serviceName, curNode->numArgs, curNode->returnType);
          print_alt_servers(outputBuffer, serviceList);
        }
        return 0;
      }
    }
    else
    {
      // We don't have this service but we know some servers that do
      sprintf(outputBuffer, "SERVER/UNKNOWN\nSERVICE/INFO&%s,%d,%s\n", curNode->serviceName, curNode->numArgs, curNode->returnType);
      print_alt_servers(outputBuffer, serviceList);
      return 0;
    }
  }
  return 1;
}

int process_request(int inputfd)
{
  // init
  int bytesRead = 0;
  char inputBuff[1024];
  memset(inputBuff, '\0', sizeof(inputBuff));
  
  // Read into the input buffer
  bytesRead = read(inputfd, inputBuff, sizeof(inputBuff)-1);
  if(bytesRead < 0)
  {
    printf("Read Error\n");
    exit(1);
  }
  inputBuff[bytesRead] = '\0';

  // Determine what kind of request this is
  if(strstr(inputBuff, "SERVER/HELLO") != NULL)
  {
    // Server Hello msg;
    // send our own hello message in return
    write(inputfd, myHelloMsg, strlen(myHelloMsg));
    // close the connection
    close(inputfd);
    // parse their hello message and add to our services table
    add_server_info(inputBuff);
  }
  else if(strstr(inputBuff, "CLIENT/REQUEST") != NULL)
  {
    // Client Request;
    char outputBuffer[2048];
    char serviceName[255];
    int argc;
    int argv[100];

    parse_client_request(inputBuff, serviceName, &argc, argv);
    printf("Client requested %s with %d arguments:\n", serviceName, argc);
    int i;
    for(i = 0; i < argc; i++)
    {
      printf("  arg %d: %d\n", i, argv[i]);
    }

    run_service(outputBuffer, serviceName, argc, argv);

    write(inputfd, outputBuffer, strlen(outputBuffer));
    close(inputfd);
  }
  else
  {
    printf("Unsupported Request Recieved\n");
    return 0;
  }

  return 1;
}

void accept_requests()
{
  int listenfd = 0;
  int connfd = 0;
  // Accept Requests Forever
  //  if msg is from a Server
  //    Add the info to my services table
  //  if msg is from a Client
  //    attempt to process the request
  // End While
  listenfd = tcp_listen(myPort);
  printf("I'm now listening on port: %d\n", myPort);
  while(1)
  {
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    process_request(connfd);
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
    printf("invalid arguments provided. Please see documentation for proper use of the server.");
    return 1;
  }

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
  printf("Using HostName: %s\n", myHostName);

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

  if(argc > 2)
  {
    // Read a configuration of other known servers
    //  For each server
    //    Send a Hello Message
    //  End For
    int i;
    char curServerString[512];
    char curServerName[255];
    int curServerPort;
    for(i = 2; i < argc; i++)
    {
      //parse the server info;
      strcpy(curServerString, argv[i]);
      parse_host_string(curServerString, curServerName, &curServerPort);

      //Introduce myself to the other servers
      //@TODO: If I make linked list thread safe, then I can do this in a multithreaded fashion.
      exchange_server_info(curServerName, curServerPort);
    }
  }

  accept_requests();
}
