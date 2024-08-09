// Nitsan BenHanoch 208585927

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

// for timing
#include <time.h>
#include <stdio.h>
#include <stddef.h>

#define PANIC(msg) do {                                                         \
  fprintf(stderr, "Runtime error in \"%s:%d\". %s\n", __FILE__, __LINE__, msg); \
  exit(EXIT_FAILURE);                                                           \
} while (0)

#define START_TIMER(name) clock_t __timer_ ## name = clock()

#define PRINT_TIMER(name) \
  printf(#name " took %f seconds\n", ((double) (clock() - __timer_ ## name)) / CLOCKS_PER_SEC)

#endif // COMMON_H
