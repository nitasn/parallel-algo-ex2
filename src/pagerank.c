#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <assert.h>

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

  START_TIMER(loop);
  for (size_t i = 0; i < N; i++) {
    output_ranks[i] = 14.7f;
  }
  PRINT_TIMER(loop);

  START_TIMER(func);
  fill_array_with(output_ranks, N, 155.6f);
  PRINT_TIMER(func);

  for (size_t i = 0; i < N; i++) {
    assert(output_ranks[i] == 155.6f);
  }

  if (iterations == 7 && output_ranks == NULL)
    ;
}