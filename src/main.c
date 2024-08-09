// #include "graph.h"

// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>

// #define D 0.15 // damping factor


// void PageRank(Graph *graph, size_t iterations, float* ranks) {
//   size_t N = graph->num_vertices;

//   for (size_t i = 0; i < N; i++) {
//     ranks[i] = 1.0 / N;
//   }

//   float *newRanks = (float *) malloc(N * sizeof(float));

//   bool *does_link_to = (bool *) calloc(N * N, sizeof(bool));

//   for (size_t i = 0; i < N; i++) {
//     for (size_t j = 0; j < N; j++) {
//       if (i == j) continue;

//       for (size_t k = 0; k < graph->neighbors_of[j].size; ++k) {
//         if (graph->neighbors_of[j].data[k] == i) {
//           does_link_to[j * N + i] = 1;
//           break;
//         }
//       }
//     }
//   }
  
//   for (size_t iter = 0; iter < iterations; iter++) {

//     // calculate nodes with outlinks to i

//     for (size_t i = 0; i < N; i++) {
      
//       // calculate i rank

//       double sumA = 0.0;
//       double sumB = 0.0;

//       for (size_t j = 0 ; j < N; j++) {
//         if (does_link_to[j * N + i] == 1) {
//           sumA += ranks[j] / graph->neighbors_of[j].size;
//         } else if (graph->neighbors_of[j].size == 0) {
//           sumB += ranks[j]/N;
//         }
//       }

//       newRanks[i] = D/N +(1-D)*(sumA+sumB);
//     }

//     for (size_t i = 0; i < N; i++) {
//       ranks[i] = newRanks[i];
//     }
//   }

//   free(newRanks);
// }

// int main(void) {
//   size_t NUM_NODES = 3;
//   size_t iterations = 100;

//   Graph *graph = createGraph(NUM_NODES);

//   addEdge(graph, 0, 1);
//   addEdge(graph, 0, 2);
//   addEdge(graph, 1, 0);
//   addEdge(graph, 1, 2);
//   addEdge(graph, 2, 0);


//   float *ranks = (float *) malloc(NUM_NODES * sizeof(float));

//   PageRank(graph, iterations, ranks);

//   for (size_t i = 0; i < NUM_NODES; i++) {
//     printf("Rank of node %lu: %f\n", i, ranks[i]);
//   }

//   return 0;
// }

#include "pagerank.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  size_t N = 2 * 1024 * 1024 + 101;
  Graph *g = createGraph(N);

  float *ranks = (float *) malloc(N * sizeof(float));

  PageRank(g, 0, ranks);

  free(ranks);
  freeGraph(g);
}