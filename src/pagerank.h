#ifndef _PAGE_RANK_
#define _PAGE_RANK_

#include "graph.h"

#include <unistd.h>

void PageRank(Graph *g, int iterations, float *rank);

#endif