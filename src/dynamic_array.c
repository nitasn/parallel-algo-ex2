#include "dynamic_array.h"
#include "common.h"

void initDynamicArray(DynamicArray *array, size_t initial_capacity) {
  if (!array) {
    PANIC("Array pointer is NULL.");
  }

  if (!initial_capacity) {
    initial_capacity = 8;
  }

  array->size = 0;
  array->capacity = initial_capacity;

  array->data = malloc(initial_capacity * sizeof(size_t));

  if (!array->data) {
    free(array);
    PANIC("Could not allocate memory.");
  }
}

void pushTo(DynamicArray *array, size_t element) {
  if (!array) {
    PANIC("Array pointer is NULL.");
  }

  if (array->size == array->capacity) {

    if (array->capacity > SIZE_MAX / 2) {
      PANIC("Array capacity overflow.");
    }

    size_t new_capacity = array->capacity > 0 ? 2 * array->capacity : 1;

    size_t *new_data = realloc(array->data, new_capacity * sizeof(size_t));

    if (!new_data) {
      PANIC("Could not allocate memory.");
    }

    array->data = new_data;
    array->capacity = new_capacity;
  }
  
  array->data[array->size++] = element;
}



void destroyDynamicArray(DynamicArray *array) {
  if (!array) {
    PANIC("Array pointer is NULL.");
  }

  free(array->data);
}
