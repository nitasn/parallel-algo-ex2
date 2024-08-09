#include "pagerank.h"

#include <stdio.h>
#include <stdlib.h>

// int main() {
//   size_t N = 2 * 1024;
//   Graph *g = createGraph(N);

//   float *ranks = (float *) malloc(N * sizeof(float));

//   PageRank(g, 0, ranks);

//   free(ranks);
//   freeGraph(g);
// }

int main(void) {
  size_t NUM_NODES = 3;
  size_t iterations = 100;

  Graph *graph = createGraph(NUM_NODES);

  addEdge(graph, 0, 1);
  addEdge(graph, 0, 2);
  addEdge(graph, 1, 0);
  addEdge(graph, 1, 2);
  addEdge(graph, 2, 0);


  float *ranks = (float *) malloc(NUM_NODES * sizeof(float));

  PageRank(graph, iterations, ranks);

  for (size_t i = 0; i < NUM_NODES; i++) {
    printf("Rank of node %lu: %f\n", i, ranks[i]);
  }

  return 0;
}