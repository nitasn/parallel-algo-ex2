#ifndef PAGE_RANK_H
#define PAGE_RANK_H

#include "graph.h"

#include <unistd.h>

void PageRank(Graph *g, int iterations, float *output_ranks);

#endif // PAGE_RANK_H
