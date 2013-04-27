/**
 * @File ll_hash_table_test.c
 * CS 470 Assignment 2
 * Ray Imber
 * Tests the functionality of the linked_list & hash_table libraries working together.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.c"
#include "linked_list.c"

void test_print(void* data)
{
  if(data != NULL)
  {
    int intData;
    intData = (*(int*)data);
    printf("    Node Data: %d\n", intData);
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
  hashTable *myHashTable;
  myHashTable = hashTable_create(20, &linkedList_free_function, &linkedList_add_function, &linkedList_remove_function);

  char test0_key[] = "test0";
  int* test0_val = NULL;
  test0_val = malloc(sizeof(int));
  (*test0_val) = 0;
  hashTable_addElement(myHashTable, test0_key, (void*)test0_val);

  int* test0_val_b = NULL;
  test0_val_b = malloc(sizeof(int));
  (*test0_val_b) = 36;
  hashTable_addElement(myHashTable, test0_key, (void*)test0_val_b);

  void* lookup_result;
  lookup_result = hashTable_lookup(myHashTable, test0_key);
  printf("lookup result for key: %s\n", test0_key);
  linkedList_printList((linkedListStruct*)lookup_result, &test_print);

  hashTable_removeElement(myHashTable, test0_key, (void*)test0_val);

  lookup_result = hashTable_lookup(myHashTable, test0_key);
  printf("lookup result for key: %s\n", test0_key);
  linkedList_printList((linkedListStruct*)lookup_result, &test_print);  

  char test1_key[] = "test1";
  int* test1_val = NULL;
  test1_val = malloc(sizeof(int));
  (*test1_val) = 1;
  hashTable_addElement(myHashTable, test1_key, (void*)test1_val);

  lookup_result = hashTable_lookup(myHashTable, test1_key);
  printf("lookup result for key: %s\n", test1_key);
  linkedList_printList((linkedListStruct*)lookup_result, &test_print);

  char test2_key[] = "test2";
  int* test2_val = NULL;
  test2_val = malloc(sizeof(int));
  (*test2_val) = 2;
  hashTable_addElement(myHashTable, test2_key, (void*)test2_val);

  lookup_result = hashTable_lookup(myHashTable, test2_key);
  printf("lookup result for key: %s\n", test2_key);
  linkedList_printList((linkedListStruct*)lookup_result, &test_print);

  char test3_key[] = "test3";
  lookup_result = hashTable_lookup(myHashTable, test3_key);
  printf("lookup result for key: %s\n", test3_key);
  linkedList_printList((linkedListStruct*)lookup_result, &test_print);

  hashTable_free(myHashTable);
  return 0;
}
