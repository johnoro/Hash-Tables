#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
typedef struct LinkedPair {
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable {
  int capacity;
  int resized;
  int num_used;
  LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL) {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char * u_str = (unsigned char *)str;

  while ((c = *u_str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

void init_hash_table(HashTable *ht, int capacity) {
  ht->capacity = capacity;
  ht->storage = calloc(capacity, sizeof(LinkedPair*));
  ht->resized = 0;
  ht->num_used = 0;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
  HashTable *ht = malloc(sizeof(HashTable));

  init_hash_table(ht, capacity);

  return ht;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
  for (int i = 0; i < ht->capacity; i++) {
    LinkedPair *curr = ht->storage[i], *next;
    while (curr) {
      next = curr->next;
      destroy_pair(curr);
      curr = next;
    }
  }

  free(ht->storage);
  free(ht);
}

void hash_table_insert(HashTable *ht, char *key, char *value);

/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
  LinkedPair **old_storage = ht->storage;
  int old_len = ht->capacity;

  init_hash_table(ht, ht->capacity*2);

  LinkedPair *curr;
  for (int i = 0; i < old_len; i++) {
    curr = old_storage[i];
    while (curr) {
      hash_table_insert(ht, curr->key, curr->value);
      curr = curr->next;
    }
  }

  ht->resized = 1;
  return ht;
}

HashTable *hash_table_shrink(HashTable *ht) {
  LinkedPair **old_storage = ht->storage;

  init_hash_table(ht, ht->capacity/2);

  LinkedPair *curr;
  for (int i = 0; i < ht->capacity; i++) {
    curr = old_storage[i];
    while (curr) {
      hash_table_insert(ht, curr->key, curr->value);
      curr = curr->next;
    }
  }

  ht->resized = 1;
  return ht;

  // HashTable *new_ht = create_hash_table(ht->capacity / 2);

  // int len = ht->capacity/2;
  // for (int i = 0; i < len; i++) {
  //   LinkedPair *curr = ht->storage[i];
  //   while (curr) {
  //     hash_table_insert(new_ht, curr->key, curr->value);
  //     curr = curr->next;
  //   }
  // }

  // destroy_hash_table(ht);

  // new_ht->resized = 1;
  // return new_ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in the existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
  unsigned int index = hash(key, ht->capacity);
  LinkedPair *pair = create_pair(key, value),
    *curr = ht->storage[index],
    *last = NULL;

  while (curr && strcmp(curr->key, key) != 0) {
    last = curr;
    curr = curr->next;
  }
  
  if (curr && last) {
    pair->next = curr->next;
    destroy_pair(curr);
    last->next = pair;
  } else {
    if (curr == NULL)
      ht->num_used++;
    pair->next = ht->storage[index];
    ht->storage[index] = pair;
  }
  
  if (ht->capacity > 0) {
    double load = (double) ht->num_used / ht->capacity;
    if (load > 0.7)
      hash_table_resize(ht);
    else if (ht->resized && load < 0.2)
      hash_table_shrink(ht);
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
  unsigned int index = hash(key, ht->capacity);
  LinkedPair *curr = ht->storage[index], *last = NULL;
  
  while (curr && strcmp(curr->key, key) != 0) {
    last = curr;
    curr = curr->next;
  }

  if (!curr) {
    perror("Key not found while trying to remove");
    return;
  }
  
  if (last)
    last->next = curr->next;
  else
    ht->storage[index] = curr->next;
  destroy_pair(curr);
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  int index = hash(key, ht->capacity);
  LinkedPair *curr = ht->storage[index];
  while (curr) {
    if (strcmp(curr->key, key) == 0)
      return curr->value;
    curr = curr->next;
  }
  return NULL;
}


#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n");
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n");

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));

  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);

  destroy_hash_table(ht);

  return 0;
}
#endif
