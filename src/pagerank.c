#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <string.h> // for memset

static threadpool pool = NULL;
static const int NUM_WORKERS = 4;

typedef struct {
  float *addr, value;
  size_t block_size;
} _memset_worker_args;

static void _memset_worker(void *args) {
  _memset_worker_args *params = (_memset_worker_args *)args;
  memset(params->addr, params->value, params->block_size);
}

static void fill_array_with(float *arr, size_t N, float value) {
  size_t chunk_size = N / NUM_WORKERS;

  _memset_worker_args *args = (_memset_worker_args *) malloc(NUM_WORKERS * sizeof(_memset_worker_args));
  if (!args) PANIC("malloc failed\n");

  for (int i = 0; i < NUM_WORKERS; ++i) {
    args[i].addr = arr + i * chunk_size;
    args[i].block_size = chunk_size;
    args[i].value = value;

    if (i + 1 == NUM_WORKERS) {
      args[i].block_size = arr + N - args[i].addr;
    }

    thpool_add_work(pool, _memset_worker, (void *) &args[i]);
  }

  thpool_wait(pool);

  free(args);
}

void PageRank(Graph *graph, int iterations, float *output_ranks) {
  size_t N = graph->num_vertices;

  if (!pool) pool = thpool_init(4);
  if (!pool) PANIC("Could not create thread pool");

  START_TIMER(loop);
  for (size_t i = 0; i < N; i++) {
    output_ranks[i] = 1.0 / N;
  }
  PRINT_TIMER(loop);

  START_TIMER(func);
  fill_array_with(output_ranks, N, 1.0f / N);
  PRINT_TIMER(func);

  if (iterations == 7 && output_ranks == NULL) {
    // do nothing
  }
}