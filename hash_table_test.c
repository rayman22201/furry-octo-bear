/**
 * @File hash_table_test.c
 * CS 470 Assignment 2
 * Ray Imber
 * Tests the functionality of the hash_table library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.c"

void test_add(void** currHash, void* newElement)
{
  printf("Inside test_add\n");
  (*currHash) = newElement;
}

void test_remove(void** currHash, void* elementToDelete)
{
  printf("Inside test_remove\n");
  (*currHash) = NULL;
}

void test_free(void* currHash)
{
  free(currHash);
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
  myHashTable = hashTable_create(20, &test_free, &test_add, &test_remove);

  //test the functionality of the hashTable library
  char test0_key[] = "test0";
  int* test0_val = NULL;
  test0_val = malloc(sizeof(int));
  (*test0_val) = 0;
  hashTable_addElement(myHashTable, test0_key, (void*)test0_val);
  
  char test1_key[] = "test1";
  int* test1_val = NULL;
  test1_val = malloc(sizeof(int));
  (*test1_val) = 1;
  hashTable_addElement(myHashTable, test1_key, (void*)test1_val);

  char test2_key[] = "test2";
  int* test2_val = NULL;
  test2_val = malloc(sizeof(int));
  (*test2_val) = 2;
  hashTable_addElement(myHashTable, test2_key, (void*)test2_val);

  void* lookup_result; 
  int lookup_val;
  lookup_result = hashTable_lookup(myHashTable, test1_key);
  lookup_val = (*(int *)(lookup_result));
  printf("lookup for key: %s = %d\n", test1_key, lookup_val);

  hashTable_removeElement(myHashTable, test1_key, (void*)test1_val);

  lookup_result = hashTable_lookup(myHashTable, test1_key);
  if(lookup_result != NULL){
    lookup_val = (*(int *)(lookup_result));
    printf("lookup for key: %s = %d\n", test1_key, lookup_val);
  }
  else
  {
    printf("No element at key %s\n", test1_key);
  }


  hashTable_free(myHashTable);

  return 0;
}
