#include "dynamic_array.h"

#include <stdio.h>


int main() {
  DynamicArray *arr = createDynamicArray(2);

  pushTo(arr, 55);
  pushTo(arr, 66);
  pushTo(arr, 77);
  pushTo(arr, 88);
  pushTo(arr, 99);

  for (size_t i = 0; i < arr->size; ++i) {
    printf("%lu ", arr->data[i]);
  }

  printf("\n");

  freeDynamicArray(arr);

  pushTo(NULL, 7);
}