#ifndef RG_HPP
#define RG_HPP

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <string>
#include <vector>
#include <map>

#define RG_MULTI_EDGE (1<<0)
#define RG_UNDIRECTED (1<<1)
#define RG_LOOP_PAIR  (1<<2)

int random_graph(std::vector<long int> &edge_v, int n_nodes, int n_edges, unsigned int seed, unsigned int opt);


#endif
