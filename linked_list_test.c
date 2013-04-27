/**
 * @File linked_list_test.c
 * CS 470 Assignment 2
 * Ray Imber
 * Tests the functionality of the linked_list library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.c"

void test_print(void* data)
{
  if(data != NULL)
  {
    char* strData;
    strData = (char*) data;
    printf("    Node Data: %s\n", strData);
  }
  else
  {
    printf("    Node Data: NULL\n");
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
  void* ptrToList;
  linkedListStruct* myList;
  myList = NULL;
  ptrToList = (void*)(&myList);

  char test0[] = "test0";
  linkedList_add_function(ptrToList, (void*)test0);

  char test1[] = "test1";
  linkedListNode* test1Node;
  test1Node = linkedList_addNode(myList, test1);

  char test2[] = "test2";
  linkedListNode* test2Node;
  test2Node = linkedList_addNode(myList, test2);

  printf("LinkedList:\n");
  linkedList_printList(myList, &test_print);

  //printf("linkedListFree\n");
  //linkedList_free_function((void*)myList);
  //printf("LinkedList:\n");
  //linkedList_printList(myList, &test_print);

  /*printf("removeNode: test1\n");
  linkedList_removeNode(myList, test1Node);
  printf("LinkedList:\n");
  linkedList_printList(myList, &test_print);*/

  printf("abstract remove function: test0\n");
  linkedList_remove_function(ptrToList, (void*)test0);
  printf("LinkedList:\n");
  linkedList_printList(myList, &test_print);

  return 0;
}
