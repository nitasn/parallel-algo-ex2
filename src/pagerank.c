#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <assert.h>
#include <stdbool.h>

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
  // It seems that usually MEMSET ISN'T WORTH PARALLELIZING. THE ISSUE SEEMS TO BE THE MEMORY BUS.
  // And, we're talking sub-seconds for GBs of data, which is the the upper limit of RAM size anyway.
  for (size_t i = 0; i < N; i++) {
    output_ranks[i] = 1.0 / N;
  }

  float *next_ranks = (float *) malloc(N * sizeof(float));

  if (!next_ranks) PANIC("malloc failed");

  bool *has_link = (bool *) calloc(N * N, sizeof(bool));
  
  for (size_t i = 0; i < N; i++) {
    for (size_t edge = 0; edge < graph->neighbors_of[i].size; ++edge) {
      size_t j = graph->neighbors_of[i].data[edge];
      has_link[i * N + j] = 1;
    }
  }
}