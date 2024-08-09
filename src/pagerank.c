#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <assert.h>
#include <stdbool.h>

#define D 0.15 // damping factor


static threadpool pool = NULL;
static const int NUM_WORKERS = 4;

static void _destroy_pool(void) {
  if (pool) thpool_destroy(pool);
}

typedef struct {
  float *addr, value;
  size_t array_size;
} worker_args;

static void worker(void *args) {
  worker_args *params = (worker_args *)args;

  
}

static void fill_array_with(float *arr, size_t N, float value) {
  size_t split = N / NUM_WORKERS;

  worker_args *args = (worker_args *) malloc(NUM_WORKERS * sizeof(worker_args));
  if (!args) PANIC("malloc failed\n");

  for (int i = 0; i < NUM_WORKERS; ++i) {
    args[i].addr = arr + i * split;
    args[i].array_size = split;
    args[i].value = value;

    if (i + 1 == NUM_WORKERS) {
      args[i].array_size = N - i * split;
    }

    thpool_add_work(pool, worker, (void *) &args[i]);
  }

  thpool_wait(pool);

  free(args);
}

void PageRank(Graph *graph, int iterations, float *ranks) {
  size_t N = graph->num_vertices;

  if (!pool) {
    pool = thpool_init(4);
    if (pool) atexit(_destroy_pool);
    else PANIC("Could not create thread pool");
  }

  // At first, I parallelized this memset loop. Then I tested it. 
  // For N = 2 * 1024 * 1024, memset takes 0.0006 seconds, and parallel_memset takes 0.0003 seconds.
  // This seemed like a vicotry! However,
  // For N = 1024 * 1024 * 1024, memset takes 0.41 seconds, and parallel_memset takes 0.95 seconds.
  // For larger N values, the problem steepens. I investigated it online. 
  // It seems that usually MEMSET ISN'T WORTH PARALLELIZING. The issue seems to be the memory bus.
  for (size_t i = 0; i < N; i++) {
    ranks[i] = 1.0 / N;
  }

  float *next_ranks = (float *) malloc(N * sizeof(float));
  if (!next_ranks) PANIC("memory allocation failed");

  DynamicArray dead_ends;
  initDynamicArray(&dead_ends, N / 4);

  // I'm not parallelizing this loop becuase it's only done once, as opposed
  // to the next loop, that runs for many iterations.
  // This loop is fast for billions for nodes, which is the size of our data.
  for (size_t i = 0; i < N; ++i) {
    if (graph->forward_links[i].size == 0) {
      pushTo(&dead_ends, i);
    }
  }

  if (iterations > 10000) fill_array_with(NULL, 0, 0);
  

  for (int iter = 0; iter < iterations; iter++) {

    double bias = 0;
    for (size_t k = 0; k < dead_ends.size; ++k) {
      size_t v = dead_ends.data[k];
      bias += ranks[v];
    }
    bias = (1 - D) * (bias / N);

    for (size_t i = 0; i < N; i++) {
      double score_i = 0.0;

      for (size_t k = 0; k < graph->backwoard_links[i].size; ++k) {
        size_t j = graph->backwoard_links[i].data[k];
        score_i += ranks[j] / graph->forward_links[j].size;
      }

      next_ranks[i] = (D / N) + (1 - D) * score_i;
    }

    for (size_t i = 0; i < N; i++) {
      ranks[i] = next_ranks[i] + bias;
    }
  }

  // todo free & destroy stuff
}