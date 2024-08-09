#include "dynamic_array.h"
#include "graph.h"

#include <stdio.h>

void bfs(Graph *graph, size_t vertex);

int main() {
  DynamicArray arr;
  initDynamicArray(&arr, 0);

  pushTo(&arr, 55);
  pushTo(&arr, 66);
  pushTo(&arr, 77);
  pushTo(&arr, 88);
  pushTo(&arr, 99);

  for (size_t i = 0; i < arr.size; ++i) {
    printf("%lu ", arr.data[i]);
  }

  printf("\n");

  destroyDynamicArray(&arr);

  ///

  Graph *graph = createGraph(6);

  addEdge(graph, 0, 1);
  addEdge(graph, 1, 2);
  addEdge(graph, 2, 3);
  addEdge(graph, 3, 4);
  addEdge(graph, 2, 4);
  addEdge(graph, 2, 5);
  addEdge(graph, 3, 4);
  addEdge(graph, 3, 5);
  addEdge(graph, 4, 5);

  bfs(graph, 0);

  freeGraph(graph);
}

void bfs(Graph *graph, size_t vertex) {
  printf("Visiting %lu\n", vertex);
  DynamicArray neighbors = graph->neighbors_arrs[vertex];
  for (size_t i = 0; i < neighbors.size; ++i) {
    bfs(graph, neighbors.data[i]);
  }
}