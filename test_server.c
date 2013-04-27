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

 
