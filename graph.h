// Nitsan BenHanoch 208585927

#ifndef GRAPH_H
#define GRAPH_H

#include "dynamic_array.h"

#include <stddef.h>

typedef struct {
  size_t num_vertices;
  DynamicArray *forward_links, *backwoard_links;
} Graph;

Graph *createGraph(size_t num_vertices);

void addEdge(Graph *graph, size_t src, size_t dst);

void freeGraph(Graph *graph);

#endif // GRAPH_H
