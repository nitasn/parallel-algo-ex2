#include "common.h"
#include "pagerank.h"
#include "threadpool.h"

#include <assert.h>
#include <stdbool.h>

#define D 0.15 // damping factor


static threadpool pool = NULL;
static const int NUM_WORKERS = 2;

static void _destroy_pool(void) {
  thpool_destroy(pool);
}

typedef struct {
  Graph *graph;
  float *ranks, *next_ranks;
  size_t from_idx, upto_idx;
  double bias;
} worker_args;

static void worker(void *args) {
  worker_args *params = (worker_args *)args;

  Graph *graph = params->graph;
  size_t N = graph->num_vertices;

  for (size_t i = params->from_idx; i < params->upto_idx; ++i) {
    double score_i = 0.0;

    for (size_t k = 0; k < graph->backwoard_links[i].size; ++k) {
      size_t j = graph->backwoard_links[i].data[k];
      // for every edge (j, i) in the graph:
      score_i += params->ranks[j] / graph->forward_links[j].size;
    }

    params->next_ranks[i] = (D / N) + (1 - D) * score_i + params->bias;
  }
}


// reusing memory: avoiding malloc on every iteration.
static worker_args *_args_arr = NULL;

static void _free_args_arr(void) {
  free(_args_arr);
}

static void parallel_page_rank_round(Graph *graph, float *ranks, float *next_ranks, double bias) {
  size_t N = graph->num_vertices;
  size_t split = N / NUM_WORKERS;

  if (!_args_arr) {
    _args_arr = (worker_args *) malloc(NUM_WORKERS * sizeof(worker_args));
    if (_args_arr) atexit(_free_args_arr);
    else PANIC("Could not allocate memory\n");
  }

  worker_args *args = _args_arr;

  for (int i = 0; i < NUM_WORKERS; ++i) {
    args[i].graph = graph;
    args[i].ranks = ranks;
    args[i].next_ranks = next_ranks;

    args[i].from_idx = i * split;
    args[i].upto_idx = (i + 1) * split;

    if (i + 1 == NUM_WORKERS) {
      args[i].upto_idx = N;
    }

    args[i].bias = bias;

    thpool_add_work(pool, worker, (void *) &args[i]);
  }

  thpool_wait(pool);
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


  for (int iter = 0; iter < iterations; ++iter) {

    double bias = 0;

    // whether to parallelize this loop is a question of how many dead_ends there are.
    // assuming their number is relatively small, the work is better be done here without overhead.
    for (size_t k = 0; k < dead_ends.size; ++k) {
      size_t v = dead_ends.data[k];
      bias += ranks[v];
    }

    bias = (1 - D) * (bias / N);

    parallel_page_rank_round(graph, ranks, next_ranks, bias);

    // see my comment above for why not to parallelize memcpy.
    // tl;dr - it's not faster for larger N values. something memory bus contention something.
    for (size_t i = 0; i < N; i++) {
      ranks[i] = next_ranks[i];
    }
  }

  // todo free & destroy stuff
}