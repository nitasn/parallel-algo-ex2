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

#include <stdbool.h>
#include <assert.h>
#include <math.h>

bool are_close(float f1, float f2) {
  const float tolerance = 0.000001f;
  return fabs(f1 - f2) < tolerance;
}

#define NUM_NODES 4

int main(void) {
  size_t iterations = 100;

  Graph *graph = createGraph(NUM_NODES);

  addEdge(graph, 0, 1);
  addEdge(graph, 0, 2);
  addEdge(graph, 1, 0);
  addEdge(graph, 1, 2);
  addEdge(graph, 2, 0);
  addEdge(graph, 2, 3);
  addEdge(graph, 1, 3);
  addEdge(graph, 0, 3);


  float *ranks = (float *) malloc(NUM_NODES * sizeof(float));

  PageRank(graph, iterations, ranks);

  float results[NUM_NODES] = { 0.257356f, 0.180600f, 0.231771f, 0.330273f };

  for (size_t i = 0; i < NUM_NODES; i++) {
    printf("Rank of node %lu: %f\n", i, ranks[i]);
    assert(are_close(ranks[i], results[i]));
  }

  return 0;
}
