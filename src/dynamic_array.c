#include "dynamic_array.h"

#include <stdlib.h>
#include <stdio.h>

#define PANIC(msg) do { \
  fprintf(stderr, "Runtime error in \"%s:%d\". %s\n", __FILE__, __LINE__, msg); \
  exit(EXIT_FAILURE); \
} while (0)

DynamicArray *createDynamicArray(size_t initial_capacity) {
  DynamicArray *array = malloc(sizeof(DynamicArray));
  if (array == NULL) {
    PANIC("Could not allocate memory.");
  }

  array->size = 0;
  array->capacity = initial_capacity;

  array->data = malloc(initial_capacity * sizeof(size_t));
  if (initial_capacity > 0 && array->data == NULL) {
    free(array);
    PANIC("Could not allocate memory.");
  }

  return array;
}

void pushTo(DynamicArray *array, size_t element) {
  if (array == NULL) {
    PANIC("Array pointer is NULL.");
  }

  if (array->size == array->capacity) {
    size_t new_capacity = array->capacity > 0 ? 2 * array->capacity : 1;

    if (array->capacity > SIZE_MAX / 2) {
      PANIC("Array capacity overflow.");
    }

    size_t *new_data = realloc(array->data, new_capacity * sizeof(size_t));
    if (new_data == NULL) {
      PANIC("Could not allocate memory.");
    }

    array->data = new_data;
    array->capacity = new_capacity;
  }
  
  array->data[array->size++] = element;
}



void freeDynamicArray(DynamicArray *array) {
  if (array != NULL) {
    free(array->data);
    free(array);
  }
}
