// rgr - Random GRaph
//
#include "rgr.hpp"

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

int random_graph_edge(std::vector<long int> &edge_v, int n_nodes, int n_edges, unsigned int seed, unsigned int opt) {
  int i, undirected_flag = 0, loop_flag = 0, multi_edge=0;
  long int r0, r1, tr, exclude[2];
  long int u, v, pos;

  std::pair<int, int> key;
  std::map< std::pair<int, int>, int> edge_flag;
  std::vector<long int> edge_choice;

  if (opt & RG_MULTI_EDGE)  { multi_edge = 1; }
  if (opt & RG_LOOP_PAIR)   { loop_flag = 1; }
  if (opt & RG_UNDIRECTED)  { undirected_flag = 1; }

  if (undirected_flag) {
    if ( n_edges >= (n_nodes*(n_nodes-1)/2) ) {
      n_edges = (n_nodes*(n_nodes-1)/2);
    }
  } else {
    if ( n_edges >= (n_nodes*(n_nodes-1)) ) {
      n_edges = (n_nodes*(n_nodes-1)/2);
    }
  }

  for (u=0; u<n_nodes; u++) {
    for (v=u+1; v<n_nodes; v++) {
      edge_choice.push_back(u);
      edge_choice.push_back(v);
      if (undirected_flag==0) {
        edge_choice.push_back(v);
        edge_choice.push_back(u);
      }
    }

    if (loop_flag) {
      edge_choice.push_back(u);
      edge_choice.push_back(u);
    }
  }

  srandom(seed);
  for (i=0; i<n_edges; i++) {
    pos = random() % ((edge_choice.size()/2) - i);
    pos += i;

    u = edge_choice[2*pos];
    edge_choice[2*pos] = edge_choice[2*i];
    edge_choice[2*i] = u;

    v = edge_choice[2*pos+1];
    edge_choice[2*pos+1] = edge_choice[2*i+1];
    edge_choice[2*i+1] = v;

    edge_v.push_back(u);
    edge_v.push_back(v);

  }

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
