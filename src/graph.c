#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

#define PANIC(msg) do { \
  fprintf(stderr, "Runtime error in \"%s:%d\". %s\n", __FILE__, __LINE__, msg); \
  exit(EXIT_FAILURE); \
} while (0)

Graph *createGraph(size_t num_vertices) {
  Graph *graph = (Graph *)malloc(sizeof(Graph));

  if (graph == NULL) {
    PANIC("Could not allocate memory.");
  }

  graph->num_vertices = num_vertices;
  graph->neighbors_arrs = malloc(num_vertices * sizeof(DynamicArray));

  if (graph->neighbors_arrs == NULL) {
    free(graph);
    PANIC("Could not allocate memory.");
  }

  for (size_t i = 0; i < num_vertices; ++i) {
    initDynamicArray(&graph->neighbors_arrs[i], 0);
  }

  return graph;
}

void addEdge(Graph *graph, size_t src, size_t dst) {
  if (graph == NULL) {
    PANIC("Graph pointer is NULL.");
  }

  if (src >= graph->num_vertices || dst >= graph->num_vertices) {
    PANIC("Invalid src or dst indices.");
  }

  pushTo(&graph->neighbors_arrs[src], dst);
}

void freeGraph(Graph *graph) {
  if (graph != NULL) {
    for (size_t i = 0; i < graph->num_vertices; ++i) {
      destroyDynamicArray(&graph->neighbors_arrs[i]);
    }

    free(graph);
  }
}