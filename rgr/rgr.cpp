// rgr - Random GRaph
//
#include "rgr.hpp"

/*
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <vector>
#include <map>
#include <string>
*/

static long int _r(long int range, long int *exclude) {
  int i, j, k, it, max_it = 100, n;
  long int r;

  if (!exclude) { return range * (rand() / (RAND_MAX + 1.0)); }

  for (n=0,i=0; exclude[i]>=0; i++) { n++; }

  for (it=0; it<max_it; it++) {

    r = range * (rand() / (RAND_MAX + 1.0));
    for (i=0; i<n; i++) {
      if (r == exclude[i]) break;
    }

    if (i<n) { continue; }

    return r;
  }

  return -1;
}

int random_graph(std::vector<long int> &edge_v, int n_nodes, int n_edges, unsigned int seed, unsigned int opt) {
  int i, undirected_flag = 0, loop_flag = 0, multi_edge=0;
  long int r0, r1, tr, exclude[2];

  std::pair<int, int> key;
  std::map< std::pair<int, int>, int> edge_flag;

  if (opt & RG_MULTI_EDGE)  { multi_edge = 1; }
  if (opt & RG_LOOP_PAIR)   { loop_flag = 1; }
  if (opt & RG_UNDIRECTED)  { undirected_flag = 1; }

  edge_v.clear();

  if (seed>0) { srand(seed); }

  for (i=0; i<n_edges; i++) {

    exclude[0] = -1;
    r0 = _r(n_nodes, exclude);

    exclude[0] = r0; exclude[1] = -1;
    r1 = _r(n_nodes, exclude);

    if ((multi_edge==0) && (undirected_flag)) {
      if (r0>r1) {
        tr = r0; r0 = r1; r1 = tr;
      }
    }

    key.first = r0;
    key.second = r1;

    if (multi_edge==0) {
      if (edge_flag.find(key) != edge_flag.end()) {
        continue;
      }
    }

    edge_flag[key] = 1;

    edge_v.push_back(r0);
    edge_v.push_back(r1);

    if ((undirected_flag==0) && (loop_flag)) {
      key.first = r1;
      key.second = r0;
      edge_flag[key] = 1;

      edge_v.push_back(r1);
      edge_v.push_back(r0);
    }

  }

  return 0;

}

