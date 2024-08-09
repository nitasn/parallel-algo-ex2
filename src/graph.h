#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

typedef struct graph_t {
  size_t size;
  size_t **neighbors_lists;
} Graph;


void addEdge(Graph *graph, size_t src, size_t dst);

Graph *createGraph(size_t size);

#endif
