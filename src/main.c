#include "pagerank.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  size_t N = 2 * 1024;
  Graph *g = createGraph(N);

  float *ranks = (float *) malloc(N * sizeof(float));

  PageRank(g, 0, ranks);

  free(ranks);
  freeGraph(g);
}