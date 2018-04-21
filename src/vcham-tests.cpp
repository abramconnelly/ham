#include "vcham.hpp"

static int int32_cmp2(const void *a, const void *b) {
  int32_t *x, *y;
  x = (int32_t *)a;
  y = (int32_t *)b;

  if (x[0] < y[0]) { return -1; }
  if (x[0] > y[0]) { return  1; }
  if (x[1] < y[1]) { return -1; }
  if (x[1] > y[1]) { return  1; }
  return 0;
}

int test_random_edges(vcham_t &g) {
  int i, j, k, r;
  int idx, s;
  int u, v;
  std::vector< int32_t > elist;

  size_t nele=0;


  for (u=0; u<g.n_vertex; u++) {
    idx = g.edge_idx[u];
    for (s=0; s<g.n_nei[u]; s++) {
      v = g._edge[idx+s];

      if (u<v) {
        elist.push_back(u);
        elist.push_back(g._edge[idx + s]);
      }
      else {
        elist.push_back(g._edge[idx + s]);
        elist.push_back(u);
      }

      nele++;

    }
  }

  qsort(&(elist[0]), nele, sizeof(int32_t)*2, int32_cmp2);

  for (i=0; i<elist.size(); i+=2) {

    // skip duplicate edge
    //
    if ((i>0) &&
        (elist[i-2] == elist[i]) &&
        (elist[i-1] == elist[i+1])) { continue; }

    r = vcham_reme(g, elist[i], elist[i+1]);
    if (r<0) { return r; }
  }

  for (u=0; u<g.n_vertex; u++) for (v=0; v<g.n_vertex; v++) {
    printf("u%i - v%i: %i\n",
        u, v, vcham_has_raw_edge(g, u, v));
  }

}

int test_rem_raw_edge(vcham_t &g) {
  int i, j, k, r;
  int idx, s;
  int u, v;
  std::vector< int32_t > elist;

  size_t nele=0;


  for (u=0; u<g.n_vertex; u++) {
    idx = g.edge_idx[u];
    for (s=0; s<g.n_nei[u]; s++) {
      v = g._edge[idx+s];

      if (u<v) {
        elist.push_back(u);
        elist.push_back(g._edge[idx + s]);
      }
      else {
        elist.push_back(g._edge[idx + s]);
        elist.push_back(u);
      }

      nele++;

    }
  }

  qsort(&(elist[0]), nele, sizeof(int32_t)*2, int32_cmp2);

  for (i=0; i<elist.size(); i+=2) {

    // skip duplicate edge
    //
    if ((i>0) &&
        (elist[i-2] == elist[i]) &&
        (elist[i-1] == elist[i+1])) { continue; }

    r = vcham_reme(g, elist[i], elist[i+1]);
    if (r<0) { return r; }
  }

  for (i=0; i<elist.size(); i+=2) {
    if (!vcham_has_raw_edge(g, elist[i], elist[i+1])) { return -1; }
    printf("u%i - v%i: %i\n",
        elist[i], elist[i+1], vcham_has_raw_edge(g, elist[i], elist[i+1]));
  }

  while (g.history.size()>0) {
    vcham_addback(g);
  }

  return vcham_check(g);
}

int test_rem_single_edges(vcham_t &g) {
  int i, j, k, r;
  int idx, s;
  int u, v;
  std::vector< int32_t > elist;

  for (u=0; u<g.n_vertex; u++) {
    idx = g.edge_idx[u];
    for (s=0; s<g.n_nei[u]; s++) {
      elist.push_back(u);
      elist.push_back(g._edge[idx + s]);
    }
  }

  for (i=0; i<elist.size(); i+=2) {
    r = vcham_reme(g, elist[i], elist[i+1]);
    if (r<0) { return r; }

    r = vcham_check(g);
    if (r<0) { return r; }

    r = vcham_addback(g);
    if (r<0) { return r; }

    r = vcham_check(g);
    if (r<0) { return r; }
  }

  return 0;
}

int test_random_stress(vcham_t &g, int n_it, double p) {
  int i, j, k, u, v, x, y;
  int u_idx, idx, v_idx;
  int v_offset;
  int it, r;

  for (it=0; it<n_it; it++) {

		if (drand48() < p) {
			if (g.history.size() == 0) { continue; }
			vcham_addback(g);
			r = vcham_check(g);
			if (r<0) { return r; }
		} else {
			u = (int)(drand48()*g.n_vertex);
			if (g.n_nei[u] == 0) { continue; }
			idx = g.edge_idx[u];
			v_offset = (int)(drand48()*g.n_nei[u]);
			v = g._edge[idx + v_offset];

			r = vcham_reme(g, u, v);
			if (r<0) { return r; }
			r = vcham_check(g);
			if (r<0) { return r; }
		}

  }

  return 0;
}

int test_prune_path_vertex() {
  vcham_t g;
  size_t hpos=0;
  int r=0, ii, maxv, n=5;
  int test_path[] = {0,1,2,3,4};
  int edge[] = {0,1,  0,2,  0,4,
                1,2,  1,3,
                2,3,  2,4,
                3,4, -1};

  maxv=-1;
  for (ii=0; edge[ii]>=0; ii+=2) {
    g.history.push_back( edge[ii] );
    g.history.push_back( edge[ii+1] );
    g.history.push_back( edge[ii+1] );
    g.history.push_back( edge[ii] );

    maxv = ( (edge[ii]   > maxv) ? edge[ii]   : maxv );
    maxv = ( (edge[ii+1] > maxv) ? edge[ii+1] : maxv );
  }
  g.n_vertex = maxv+1;
  g.n_edge = ii/2;

  vcham_init_from_history(g);

  g.visited[1]=1;
  g.visited[2]=1;
  g.visited[3]=1;

  hpos = g.history.size();

  vcham_prune_path_vertex(g, 2);

  if ((r=vcham_check(g))!=0) { return r; }

  if (!vcham_has_edge(g,1,2)) { return -1; }
  if (!vcham_has_edge(g,2,1)) { return -1; }
  if (!vcham_has_edge(g,2,3)) { return -1; }
  if (!vcham_has_edge(g,3,2)) { return -1; }

  if (vcham_has_edge(g,2,4)) { return -1; }
  if (vcham_has_edge(g,4,2)) { return -1; }
  if (vcham_has_edge(g,0,2)) { return -1; }
  if (vcham_has_edge(g,2,0)) { return -1; }

  vcham_unwind_history(g, hpos);

  if ((r=vcham_check(g))!=0) { return r; }

  return 0;
}

int test_prune_path_vertex0() {
  int r=0;

  r = test_prune_path_vertex0();
  if (r<0) { return r; }

  return 0;
}

// ---
// ---
// ---

int test_prune_between_degree_two0() {
  vcham_t g;
  size_t hpos=0;
  int r=0, ii, maxv, n=5;
  int test_path[] = {0,1,2,3,4};
  int edge[] = {0,1,  0,2,  0,4,
                1,2,
                2,3,  2,4,
                3,4, -1};

  maxv=-1;
  for (ii=0; edge[ii]>=0; ii+=2) {
    g.history.push_back( edge[ii] );
    g.history.push_back( edge[ii+1] );
    g.history.push_back( edge[ii+1] );
    g.history.push_back( edge[ii] );

    maxv = ( (edge[ii]   > maxv) ? edge[ii]   : maxv );
    maxv = ( (edge[ii+1] > maxv) ? edge[ii+1] : maxv );
  }
  g.n_vertex = maxv+1;
  g.n_edge = ii/2;

  vcham_init_from_history(g);

  hpos = g.history.size();

  vcham_prune_between_degree_two(g);

  if ((r=vcham_check(g))!=0) { return r; }

  if (!vcham_has_edge(g,0,1)) { return -1; }
  if (!vcham_has_edge(g,1,0)) { return -1; }
  if (!vcham_has_edge(g,1,2)) { return -1; }
  if (!vcham_has_edge(g,2,1)) { return -1; }
  if (!vcham_has_edge(g,2,3)) { return -1; }
  if (!vcham_has_edge(g,3,2)) { return -1; }
  if (!vcham_has_edge(g,3,4)) { return -1; }
  if (!vcham_has_edge(g,4,3)) { return -1; }
  if (!vcham_has_edge(g,0,4)) { return -1; }
  if (!vcham_has_edge(g,4,0)) { return -1; }


  if (vcham_has_edge(g,0,2)) { return -1; }
  if (vcham_has_edge(g,2,0)) { return -1; }
  if (vcham_has_edge(g,2,4)) { return -1; }
  if (vcham_has_edge(g,4,2)) { return -1; }

  vcham_unwind_history(g, hpos);

  if ((r=vcham_check(g))!=0) { return r; }

  return 0;
}

int test_prune_between_degree_two() {
  int r=0;

  if ((r=test_prune_between_degree_two0())!=0) { return r; }

  return 0;
}

int test_prune_path_degree_two_edge(vcham_t &g, int n_it) {
  return 0;
}

int test_ccfill_inst(int n_vertex, std::vector< int32_t > &edge) {
  int i;
  int32_t u;
  std::vector< int32_t > v_group;
  vcham_t g;

  g.n_vertex = n_vertex;
  g.n_edge = (int)(edge.size()/2);
  for (i=0; i<edge.size(); i+=2) {
    g.history.push_back(edge[i]);
    g.history.push_back(edge[i+1]);
    g.history.push_back(edge[i+1]);
    g.history.push_back(edge[i]);
  }
  vcham_init_from_history(g);

  vcham_fprint_dot_undirected(stdout, g);

  for (u=0; u<g.n_vertex; u++) {
    v_group.push_back(-1);
  }

  for (u=0; u<g.n_vertex; u++) {
    vcham_ccfill(g, u, u, v_group);
  }

  for (i=0; i<v_group.size(); i++) {
    printf(" %i", (int)v_group[i]);
  }
  printf("\n");

  return 0;
}

int test_ccfill() {
  int r, n_vertex = 5;
  std::vector< int32_t > edge{ 0, 1,  2, 3, 3, 4 },
    edge1{ 0, 1, 0, 2, 3, 4, 1, 4 };

  r=test_ccfill_inst(n_vertex, edge);
  r=test_ccfill_inst(n_vertex, edge1);

  return r;
}
