#include "graph.h"
#include "common.h"

Graph *createGraph(size_t num_vertices) {
  Graph *graph = (Graph *) malloc(sizeof(Graph));

  if (!graph) {
    PANIC("Could not allocate memory.");
  }

  graph->num_vertices = num_vertices;
  graph->neighbors_of = malloc(num_vertices * sizeof(DynamicArray));

  if (!graph->neighbors_of) {
    free(graph);
    PANIC("Could not allocate memory.");
  }

  for (size_t i = 0; i < num_vertices; ++i) {
    initDynamicArray(&graph->neighbors_of[i], 0);
  }

  return graph;
}

void addEdge(Graph *graph, size_t src, size_t dst) {
  if (!graph) {
    PANIC("Graph pointer is NULL.");
  }

  if (src >= graph->num_vertices || dst >= graph->num_vertices) {
    PANIC("Invalid src or dst indices.");
  }

  pushTo(&graph->neighbors_of[src], dst);
}

void freeGraph(Graph *graph) {
  if (!graph) return;
  
  for (size_t i = 0; i < graph->num_vertices; ++i) {
    destroyDynamicArray(&graph->neighbors_of[i]);
  }

  free(graph);
}