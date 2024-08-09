#ifndef GRAPH_H
#define GRAPH_H

#include "dynamic_array.h"

#include <stddef.h>

typedef struct graph_t {
  size_t num_vertices;
  DynamicArray *neighbors_of;
} Graph;


Graph *createGraph(size_t num_vertices);

void addEdge(Graph *graph, size_t src, size_t dst);

void freeGraph(Graph *graph);

#endif
