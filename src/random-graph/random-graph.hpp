#ifndef RANDOM_GRAPH_HPP
#define RANDOM_GRAPH_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include <vector>
#include <string>


enum RG_TYPE_ENUM {
  RG_TYPE_ER_N_P=0,
  RG_TYPE_ER_N_M,
  RG_TYPE_DEG_DIST_EFFORT
};

typedef struct random_graph_type {
  uint32_t n, m;
  double p;
  long int seed;
  std::vector< uint32_t > deg_dist;
  int type;

  std::vector< int32_t > edge;

  int allow_multi_edge;
  int allow_self_loop;
  int directed;

  random_graph_type() : seed(0) , n(0), m(0), p(0.0), allow_multi_edge(0), allow_self_loop(0), directed(0) { }
} random_graph_t;

int rg_erdos_renyi_G_n_p(random_graph_t &rg, uint32_t n, double p);
int rg_erdos_renyi_G_n_m(random_graph_t &rg, uint32_t n, uint32_t m);

int rg_deg_dist_best_effort(random_graph_t &rg, uint32_t n, uint32_t m);

void rg_fprint_dimacs(FILE *fp, random_graph_t &rg);
void rg_fprint_dot(FILE *fp, random_graph_t &rg);

#endif
