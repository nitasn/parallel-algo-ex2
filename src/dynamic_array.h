#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

typedef struct dynamic_array_t {
  size_t size, capacity;
  size_t *data;
} DynamicArray;

DynamicArray *createDynamicArray(size_t initial_capacity);

void pushTo(DynamicArray *array, size_t element);

void freeDynamicArray(DynamicArray *array);

#endif
