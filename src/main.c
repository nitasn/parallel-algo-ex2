#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

#define D 0.15 // damping factor


void PageRank(Graph *graph, size_t iterations, float* ranks) {
  size_t N = graph->num_vertices;

  for (size_t i = 0; i < N; i++) {
    ranks[i] = 1.0 / N;
  } 

  float *newRanks = (float *) malloc(N * sizeof(float));
  
  for (size_t iter = 0; iter < iterations; iter++) {

    // calculate nodes with outlinks to i

    for (size_t i = 0; i < N; i++) {
      size_t* out2i = (size_t *) calloc(N, sizeof(size_t));
      
      for (size_t j = 0; j < N; j++) {
        if (j == i) continue;
        for (size_t k = 0; k < graph->neighbors_of[j].size; ++k) {
          if (graph->neighbors_of[j].data[k] == i) {
            out2i[j] = 1;
            break;
          }
        }
      }
      
      // calculate i rank

      double sumA = 0.0;
      double sumB = 0.0;

      for (size_t j = 0 ; j < N; j++) {
        if(out2i[j] == 1) {
          sumA += ranks[j] / graph->neighbors_of[j].size;
        } else if (graph->neighbors_of[j].size == 0) {
          sumB += ranks[j]/N;
        }
      }

      newRanks[i] = D/N +(1-D)*(sumA+sumB);
    }

    for (size_t i = 0; i < N; i++) {
      ranks[i] = newRanks[i];
    }
  }

  free(newRanks);
}

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
