/**
 * @File client.c
 * CS 470 Assignment 2
 * Ray Imber
 * Implements the Client described in the assignment
 *
 * Data Structures I need:
 *   servers list = linked list (maybe with lazy delete if I want to be fancy)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char serverName[255];
  int serverPort; // In Network Byte Order.
} ServerNode;

/**
 * Main
 * @param  argc argument count
 * @param  argv argument array
 * @return      boolean success or failure
 */
int main( int argc, const char* argv[] )
{
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