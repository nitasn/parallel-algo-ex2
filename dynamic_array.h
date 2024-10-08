// Nitsan BenHanoch 208585927

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

typedef struct {
  size_t size, capacity;
  size_t *data;
} DynamicArray;

void initDynamicArray(DynamicArray *array, size_t initial_capacity);

void pushTo(DynamicArray *array, size_t element);

void destroyDynamicArray(DynamicArray *array);

#endif // DYNAMIC_ARRAY_H
