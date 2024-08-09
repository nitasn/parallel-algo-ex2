#include "graph.h"
#include "common.h"

Graph *createGraph(size_t num_vertices) {
  Graph *graph = (Graph *) malloc(sizeof(Graph));

  if (!graph) {
    PANIC("Could not allocate memory.");
  }

  graph->num_vertices = num_vertices;

  graph->forward_links = malloc(num_vertices * sizeof(DynamicArray));
  graph->backwoard_links = malloc(num_vertices * sizeof(DynamicArray));

  if (!graph->forward_links || !graph->backwoard_links) {
    free(graph->forward_links);
    free(graph->backwoard_links);
    free(graph);
    PANIC("Could not allocate memory.");
  }

  for (size_t i = 0; i < num_vertices; ++i) {
    initDynamicArray(&graph->forward_links[i], 0);
    initDynamicArray(&graph->backwoard_links[i], 0);
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

  pushTo(&graph->forward_links[src], dst);
  pushTo(&graph->backwoard_links[dst], src);
}

void freeGraph(Graph *graph) {
  if (!graph) return;
  
  for (size_t i = 0; i < graph->num_vertices; ++i) {
    destroyDynamicArray(&graph->forward_links[i]);
    destroyDynamicArray(&graph->backwoard_links[i]);
  }

  free(graph);
}