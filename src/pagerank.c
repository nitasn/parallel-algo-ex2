#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <assert.h>
#include <stdbool.h>

#define D 0.15 // damping factor


static threadpool pool = NULL;
static const int NUM_WORKERS = 4;

static void destroy_pool(void) {
  if (pool) {
    thpool_destroy(pool);
  }
}

typedef struct {
  float *addr, value;
  size_t array_size;
} _memset_worker_args;

static void _memset_worker(void *args) {
  _memset_worker_args *params = (_memset_worker_args *)args;

  for (size_t i = 0; i < params->array_size; ++i) {
    params->addr[i] = params->value;
  }
}

static void fill_array_with(float *arr, size_t N, float value) {
  size_t split = N / NUM_WORKERS;

  _memset_worker_args *args = (_memset_worker_args *) malloc(NUM_WORKERS * sizeof(_memset_worker_args));
  if (!args) PANIC("malloc failed\n");

  for (int i = 0; i < NUM_WORKERS; ++i) {
    args[i].addr = arr + i * split;
    args[i].array_size = split;
    args[i].value = value;

    if (i + 1 == NUM_WORKERS) {
      args[i].array_size = N - i * split;
    }

    thpool_add_work(pool, _memset_worker, (void *) &args[i]);
  }

  thpool_wait(pool);

  free(args);
}

void PageRank(Graph *graph, int iterations, float *output_ranks) {
  size_t N = graph->num_vertices;

  if (!pool) {
    pool = thpool_init(4);
    if (pool) atexit(destroy_pool);
    else PANIC("Could not create thread pool");
  }

  // At first, I parallelized this memset loop. Then I tested it. 
  // For N = 2 * 1024 * 1024, memset takes 0.0006 seconds, and parallel_memset takes 0.0003 seconds.
  // This seemed like a vicotry! However,
  // For N = 1024 * 1024 * 1024, memset takes 0.41 seconds, and parallel_memset takes 0.95 seconds.
  // For larger N values, the problem steepens. I investigated it online. 
  // It seems that usually MEMSET ISN'T WORTH PARALLELIZING. The issue seems to be the memory bus.
  for (size_t i = 0; i < N; i++) {
    output_ranks[i] = 1.0 / N;
  }

  float *next_ranks = (float *) malloc(N * sizeof(float));
  bool *adjacency_matrix = (bool *) calloc(N * N, sizeof(bool));

  if (!next_ranks || !adjacency_matrix) PANIC("memory allocation failed");

  #define edge_exists(i, j) adjacency_matrix[(i) * N + (j)]

  for (size_t i = 0; i < N; i++) {
    for (size_t edge = 0; edge < graph->neighbors_of[i].size; ++edge) {
      size_t j = graph->neighbors_of[i].data[edge];
      edge_exists(i, j) = true;
    }
  }

  DynamicArray loners;
  initDynamicArray(&loners, N / 4);

  for (size_t i = 0; i < N; ++i) {
    if (graph->neighbors_of[i].size == 0) {
      pushTo(&loners, i);
    }
  }

  if (iterations > 10000) fill_array_with(NULL, 0, 0);
  

  for (int iter = 0; iter < iterations; iter++) {

    double sum_loner_ranks = 0;
    for (size_t i = 0; i < loners.size; ++i) {
      size_t v = loners.data[i];
      sum_loner_ranks += output_ranks[v];
    }
    sum_loner_ranks /= N;

    for (size_t i = 0; i < N; i++) {
      
      double sumA = 0.0;

      for (size_t j = 0 ; j < N; j++) {
        if (edge_exists(j, i)) {
          sumA += output_ranks[j] / graph->neighbors_of[j].size;
        } 
      }

      next_ranks[i] = D / N + (1 - D) * (sumA + sum_loner_ranks);
    }

    for (size_t i = 0; i < N; i++) {
      output_ranks[i] = next_ranks[i];
    }
  }

  // todo free & destroy stuff
}