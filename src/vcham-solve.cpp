#include "vcham.hpp"

//#define VERBOSE_SOLVE_PRINT

int cmp_int32(const void *a, const void *b) {
  int32_t *x, *y;
  x = (int32_t *)a;
  y = (int32_t *)b;

  if ((*x) < (*y)) { return -1; }
  if ((*x) > (*y)) { return  1; }
  return 0;
}

int cmp_int32_2(const void *a, const void *b) {
  int32_t *x, *y;
  x = (int32_t *)a;
  y = (int32_t *)b;

  if (x[0] < y[0]) { return -1; }
  if (x[0] > y[0]) { return  1; }
  return 0;
}


// Check validity of Hamiltonian cycle
// `cycle_flag` is 1 for cycle checking
// `path` structure should hold vertex list
// `n_nei` doesn't need to be correct as the `vcham_has_raw_edge` is used
//   to test for vertex connectivity.
//
// 1 - valid Hamiltonian cycle
// 0 - not a valid cycle
//
int vcham_solution_check(vcham_t &g, int cycle_flag) {
  int i;
  std::vector<int> visited;

  for (i=0; i<g.n_vertex; i++) { visited.push_back(0); }

  if (g.path.size() != g.n_vertex) { return 0; }
  visited[ g.path[0] ] = 1;
  for (i=1; i<g.path.size(); i++) {
    visited[ g.path[i] ] = 1;
    if (!vcham_has_raw_edge(g, g.path[i], g.path[i-1])) { return 0; }
  }
  if (cycle_flag) {
    if (!vcham_has_raw_edge(g, g.path[0], g.path[ g.path.size()-1 ])) { return 0; }
  }

  for (i=0; i<visited.size(); i++) {
    if (visited[i] == 0) { return 0; }
  }

  return 1;
}

int vcham_cycle_check(vcham_t &g) { vcham_solution_check(g, 1); }
int vcham_path_check(vcham_t &g) { vcham_solution_check(g, 0); }

//--

int vcham_ham_solve_simple_r(vcham_t &g, int32_t u, int64_t bound) {
  int i, j, k, r=0;
  int32_t u_idx;
  std::vector< int32_t > nei;

  if (bound==0) { return -1; }

  if (g.path.size() == g.n_vertex) {
    if (vcham_has_raw_edge(g, g.path[0], g.path[g.n_vertex-1])) { return 1; }
    return 0;
  }

  u_idx = g.edge_idx[u];
  for (i=0; i<g.n_nei[u]; i++) {
    nei.push_back(g._edge[u_idx+i]);
  }

  for (i=0; i<nei.size(); i++) {

    if (g.visited[nei[i]]) { continue; }

    g.visited[nei[i]] = 1;
    g.path.push_back(nei[i]);

    r = vcham_ham_solve_simple_r(g, nei[i], bound-1);

    if (r!=0) { return r; }

    g.path.pop_back();
    g.visited[nei[i]] = 0;
  }

  return r;
}

int vcham_ham_solve_simple(vcham_t &g) {
  int i, r;
  int start_v=0;

  g.visited.clear();
  for (i=0; i<g.n_vertex; i++) { g.visited.push_back(0); }

  g.path.clear();
  g.path.push_back(start_v);
  g.visited[start_v] = 1;

  r = vcham_ham_solve_simple_r(g, start_v, -1);

  if (r!=1) {
    g.path.pop_back();
    g.visited[start_v]=1;
  }

  return r;
}

//--

// remove all edges on a path vertex `u` that don't
// fall on the path.
// Uses `visited` to determine which verticies are
// on the path.
//
// 1  - pruned edges
// 0  - no pruned edges
// -1 - error
//
int vcham_prune_path_vertex(vcham_t &g, int32_t u) {
  int32_t v;
  int i, u_idx, path_nei=0, has_pruned=1;
  std::vector< int32_t > vrem;

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("## prune_vertex u%i\n", (int)u);
#endif

  // reme alters n_nei, _edge, etc., so
  // save the edges to be pruned in a vector
  // to be removed after.
  //
  u_idx = g.edge_idx[u];
  for (i=0; i<g.n_nei[u]; i++) {
    v = g._edge[u_idx+i];

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## prune_vertex: considering u%i-v%i\n", (int)u, (int)v);
#endif

    if (g.visited[v]) {

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## prune_vertex: u%i-v%i, v%i already visited, skipping\n", (int)u, (int)v, (int)v);
#endif

      path_nei++;
      continue;
    }

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## prune_vertex: pruning u%i-v%i\n", (int)u, (int)v);
#endif

    vrem.push_back(v);
    has_pruned=1;
  }

  // basic sanity check
  //
  if (path_nei!=2) {

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## prune_vertex SANITY path_nei %i\n", (int)path_nei);
#endif

    return -1;
  }

  // now we can prune them
  //
  for (i=0; i<vrem.size(); i++) {
    vcham_reme(g, u, vrem[i]);
  }

  return has_pruned;
}

// Unwind our edge removal by replaying information stored
// in history up to the `history_size` point.
//
int vcham_unwind_history(vcham_t &g, size_t history_size) {
  size_t i, n, r;
  n = (g.history.size() - history_size) / (g.history_stride);

  //for (i=0; i<n; i++) {
  for (i=0; i<n; i++) {
    r=vcham_addback(g);
    if (r<0) { return r; }
  }
  return 0;
}

// simple first try
// WIP
//
int vcham_prune_between_degree_two(vcham_t &g) {
  int i, j, k, idx;
  int32_t u, v, nei_deg2[2];
  static std::vector< int > deg_common;
  std::vector< int32_t > prune_v;
  int has_pruned=0;

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune between deg2...\n");
#endif

  // `deg_common` holds the number of neighbors
  // that have degree 2 verticies.
  // static because we don't want to keep reallocating
  // every time.
  //
  if (deg_common.size() < g.n_vertex) {
    deg_common.resize(g.n_vertex);
  }
  for (u=0; u<g.n_vertex; u++) { deg_common[u] = 0; }

  // Create the count of degree 2 neighbors
  // checking for any vertex of less than degree 2
  // on the way.
  //
  for (u=0; u<g.n_vertex; u++) {
    if (g.n_nei[u] < 2) { return -1; }
    if (g.n_nei[u] == 2) {
      idx = g.edge_idx[u];
      deg_common[g._edge[idx]]++;
      deg_common[g._edge[idx+1]]++;
      continue;
    }
  }

  // If we have less than 2 degree 2 neighbors, ignore.
  // If the degree of this vertex is 2, ignore.
  // If the number of degree 2 neighbors is more than
  //   2, no cycle is possible.
  // If the `deg_common` entry is exactly 2, that is
  //   we have exactly two neighbors of degree 2,
  //   then we can prune any additional edges.
  //
  //
  for (u=0; u<g.n_vertex; u++) {
    if (deg_common[u] < 2) { continue; }
    if (g.n_nei[u] == 2) { continue; }
    if (deg_common[u] > 2) { return -1; }

    //sanity error
    if (g.n_nei[u] < 2) { return -2; }

    prune_v.clear();
    idx = g.edge_idx[u];
    for (i=0; i<g.n_nei[u]; i++) {
      v = g._edge[idx+i];
      if (g.n_nei[v]==2) { continue; }
      prune_v.push_back(v);
    }

    for (i=0; i<prune_v.size(); i++) {
      has_pruned=1;
      vcham_reme(g, u, prune_v[i]);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("### pruned between-deg-2 u%i-v%i\n", (int)u, (int)prune_v[i]);
#endif
    }

  }

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune between deg 2 end..\n");
#endif

  return has_pruned;
}

// simple first try
// WIP
//
// if there's a chain of linked degree two vertices
// with dangling (non degree 2) vertices that are connected
// and not the full length of the graph, we can remove the edge
// linking them as no cycle can pass through.
//
inline int vcham_prune_path_degree_two_edge(vcham_t &g) {
  int i, j, k, dn, idx;
  int32_t u, v, nei_deg2[2], v_l, v_r;
  static std::vector< int > considered;
  std::vector< int32_t > path_deg2;
  int has_pruned=0, path_len=0;

  if (considered.size() < g.n_vertex) {
    considered.resize(g.n_vertex);
  }
  for (u=0; u<g.n_vertex; u++) { considered[u] = 0; }

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune path deg2...\n");
#endif

  for (u=0; u<g.n_vertex; u++) {
    if (g.n_nei[u]!=2) { continue; }
    if (considered[u] != 0) { continue; }

    considered[u] = 1;
    idx = g.edge_idx[u];
    v_l = g._edge[idx];
    v_r = g._edge[idx+1];
    path_len=1;

    while ( (path_len < g.n_vertex) && (g.n_nei[v_l] == 2) ) {
      path_len++;
      considered[v_l] = 1;
      idx = g.edge_idx[v_l];
      v = g._edge[idx];
      if (considered[v] == 0) { v_l = v; continue; }
      v_l = g._edge[idx+1];
    }

    while ( (path_len < g.n_vertex) && (g.n_nei[v_r] == 2) ) {
      path_len++;
      considered[v_r] = 1;
      idx = g.edge_idx[v_r];
      v = g._edge[idx];
      if (considered[v] == 0) { v_r=v; continue; }
      v_r = g._edge[idx+1];
    }

    if ( (path_len < g.n_vertex) &&
          vcham_has_edge(g, v_l, v_r) ) {
      //has_pruned=1;
      vcham_reme(g, v_l, v_r);

#ifdef VERBOSE_SOLVE_PRINT
      printf("### pruned path deg2 u%i-v%i, returning...\n", (int)v_l, (int)v_r);
#endif

      return 1;
    }

  }

#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune path deg2, nothing to be done\n");
#endif

  //return has_pruned;
  return 0;
}


// simple first try
// WIP
//
int vcham_prune_path_degree_two_edge__slow(vcham_t &g) {
  int i, j, k, dn, idx;
  int32_t u, v, nei_deg2[2], v_l, v_r;
  static std::vector< int > considered;
  std::vector< int32_t > path_deg2;
  int has_pruned=0, path_len=0;

  if (considered.size() < g.n_vertex) {
    considered.resize(g.n_vertex);
  }
  for (u=0; u<g.n_vertex; u++) { considered[u] = 0; }

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune path deg2...\n");
#endif

  for (u=0; u<g.n_vertex; u++) {
    if (g.n_nei[u]!=2) { continue; }
    if (considered[u] != 0) { continue; }

    considered[u] = 1;
    idx = g.edge_idx[u];
    v_l = g._edge[idx];
    v_r = g._edge[idx+1];
    path_len=1;

    while ( (path_len < g.n_vertex) && (g.n_nei[v_l] == 2) ) {
      path_len++;
      considered[v_l] = 1;
      idx = g.edge_idx[v_l];
      v = g._edge[idx];
      if (considered[v] == 0) { v_l = v; continue; }
      v_l = g._edge[idx+1];
    }

    while ( (path_len < g.n_vertex) && (g.n_nei[v_r] == 2) ) {
      path_len++;
      considered[v_r] = 1;
      idx = g.edge_idx[v_r];
      v = g._edge[idx];
      if (considered[v] == 0) { v_r=v; continue; }
      v_r = g._edge[idx+1];
    }

    if ( (path_len < g.n_vertex) &&
          vcham_has_edge(g, v_l, v_r) ) {
      //has_pruned=1;
      vcham_reme(g, v_l, v_r);

#ifdef VERBOSE_SOLVE_PRINT
      printf("### pruned path deg2 u%i-v%i, returning...\n", (int)v_l, (int)v_r);
#endif

      return 1;
    }

  }

#ifdef VERBOSE_SOLVE_PRINT
  printf("### prune path deg2, nothing to be done\n");
#endif

  //return has_pruned;
  return 0;
}


// "Culberson Vandegriend" solver
// - check for end condition
// - push the list of neighbors to the current vertex
// - sort by degree, least first ascending
// - add the next vertex in the path
// - prune edges from path vertex
// - check for middle of degree 2 vertex and prune
// - check for path of degree two and prune sub-cycle edge
//
// returns:
//
//  1 - found solution
//  0 - no solution
// -1 - unknown
//
int vcham_solve_r(vcham_t &g, int32_t u, int64_t &bound) {
  int i, j, k, r=0, still_pruning, err_ret=0;
  int nei_idx;
  size_t sz, n;
  int32_t u_idx, v;
  std::vector< int32_t > nei;
  size_t history_size;

  if (bound==0) { return -1; }

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("\n\n");
  printf("## u%i, bound %i\n", (int)u, (int)bound);
  printf("## path:");
  for (i=0; i<g.path.size(); i++) { printf(" %i", (int)g.path[i]); }
  printf("\n");
  printf("## visited:");
  for (i=0; i<g.visited.size(); i++) {

    if ((i%20)==0) { printf("\n## %4i:", i); }
    if (g.visited[i]) {
      printf(" %i", g.visited[i]);
    } else {
      printf(" .");
    }


    //printf(" (v%i %i)", i, g.visited[i]);
  }
  printf("\n");
  //vcham_fprint_dot_undirected(stdout, g);
#endif

  // Check end condition
  //
  if (g.path.size() == g.n_vertex) {

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## g.path.size() == g.n_vertex (%i)\n", (int)g.n_vertex);
#endif

    if (vcham_has_raw_edge(g, g.path[0], g.path[g.n_vertex-1])) { return 1; }

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## ...nope\n");
#endif

    printf("...?\n");

    return 0;
  }

  // Push the list of neighbors for the vertex `u`.
  // Pruning alters this neighbor list so keep a local list here.
  // Sort by degree ascending
  //

  u_idx = g.edge_idx[u];

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("## pushing neighbor list for u%i (u_idx %i) (n_nei %i)\n", (int)u, (int)u_idx, (int)g.n_nei[u]);
#endif

  for (i=0; i<g.n_nei[u]; i++) {
    v = g._edge[u_idx+i];

    if (g.visited[v]) {

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## skipping u%i - v%i\n", (int)u, (int)v);
#endif

      continue;
    }

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## u%i - v%i (deg v%i = %i)\n", (int)u, (int)v, (int)v, (int)g.n_nei[v]);
#endif

    nei.push_back(g.n_nei[v]);
    nei.push_back(v);
  }
  qsort( &(nei[0]), nei.size()/2, sizeof(uint32_t)*2, cmp_int32_2);

  //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
  printf("## sorted by deg:");
  for (i=0; i<nei.size(); i+=2) { printf(" v%i (deg %i)", nei[i+1], nei[i]); }
  printf("\n");
#endif

  // Try each neighbor
  //
  for (nei_idx=0; nei_idx<nei.size(); nei_idx+=2) {

    // v is the next vertex to recur on.
    // If we've already visited it, skip over it (this
    // really shouldn't happen?).
    //
    v = nei[nei_idx+1];
    if (g.visited[v]) {

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("#### !!!! already visited %i->%i\n", u, v);
#endif

      continue;
    }

    //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
    printf("## adding v%i to path (from u%i-v%i)\n", (int)v, (int)u, (int)v);
#endif

    // Save our history position
    //
    history_size = g.history.size();

    g.visited[v] = 1;
    g.path.push_back(v);

    sz = g.path.size();
    if (sz > 2) {

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## PRUNE path...\n");
#endif

      // take vertex before current vertex on path as
      // pruning vertex
      r = vcham_prune_path_vertex(g, g.path[sz-2]);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## ...got %i, continuing...\n", r);
#endif

      if (r<0) {
        err_ret = vcham_unwind_history(g, history_size);
        if (err_ret < 0) {

          //DEBUG
          printf("unwind error (a)\n"); fflush(stdout);

          g.err_msg = "unwind error (a)"; return -1;
        }
        g.path.pop_back();
        g.visited[v] = 0;
        return 0;
      }
    }

    // Keep trying to prune until we're all done.
    //
    do {
      still_pruning=0;

      r = vcham_prune_between_degree_two(g);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## vcham_prune_between_degree_two got: %i\n", r);
#endif


      if (r<0) {
        err_ret = vcham_unwind_history(g, history_size);
        if (err_ret < 0) {

          //DEBUG
          printf("unwinde error(b)\n"); fflush(stdout);

          g.err_msg = "unwind error (b)"; return -1;
        }
        g.path.pop_back();
        g.visited[v] = 0;
        return 0;
      }
      if (r!=0) { still_pruning = 1; }

      r = vcham_prune_path_degree_two_edge(g);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## vcham_prune_path_degree_two_edge got: %i\n", r);
#endif


      if (r<0) {
        err_ret = vcham_unwind_history(g, history_size);
        if (err_ret < 0) {

          //DEBUG
          printf("unwinde error(c)\n"); fflush(stdout);

          g.err_msg = "unwind error (c)"; return -1;
        }
        g.path.pop_back();
        g.visited[v] = 0;
        return 0;
      }
      if (r!=0) { still_pruning = 1; }

    } while (still_pruning);

    bound--;
    r = vcham_solve_r(g, v, bound);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## recur vcham_solve_r got: %i\n", r);
#endif


    if (r!=0) {

      //DEBUG
      //printf("###r! %i\n", r);

      return r;
    }

    err_ret = vcham_unwind_history(g, history_size);

      //DEBUG
#ifdef VERBOSE_SOLVE_PRINT
      printf("## unwinding (%i)\n", err_ret);
#endif


    if (err_ret < 0) {

      //DEBUG
      printf("unwind error (d)\n"); fflush(stdout);

      g.err_msg = "unwind error (d)"; return -1;
    }
    g.path.pop_back();
    g.visited[v] = 0;

  }

  return r;
}

int vcham_solve(vcham_t &g) {
  int32_t u=0;
  int64_t bound=-1;
  g.path.push_back(u);
  g.visited[u] = 1;
  return vcham_solve_r(g, u, bound);
}

/*
int vcham_connected_components(vcham_t &g) {
  int i, u_idx;
  int32_t u, v, group;
  std::vector< int32_t > v_group;

  for (i=0; i<g.n_vertex; i++) { v_group.push_back(-1); }

  group=0;
  for (u=0; u<g.n_vertex; u++) {
    if (v_group[u] >= 0) { continue; }

    v_group[u] = group;

    u_idx = g.edge_idx[u];
    for (i=0; i<g.n_nei[u]; i++) {
      v = g._edge[u_idx+i];
      vcham_fill_connected_component(g, group, v, v_group);
    }

  }
}
*/

// if `u` isn't already named, name `u` with group name `group` and recusively
// go through and name it's neighbors.
// Since the graph is not being changed while we label, we shouldn't get into
// a situation where we run into another vertex that's already been named
// with another group name.
//
void vcham_ccfill(vcham_t &g, int32_t u, int32_t group, std::vector< int32_t > &v_group) {
  int i, u_idx;
  int32_t v;
  std::vector< int32_t > vv;
  if (v_group[u] >= 0) { return; }

  // populate vertex with group labelling
  //
  v_group[u] = group;

  // recusively go through each of it's neighbors
  //
  for (i=0; i<g.n_nei[u]; i++) {
    u_idx = g.edge_idx[u];
    v = g._edge[u_idx+i];
    if (v_group[v] >= 0) { continue; }
    vcham_ccfill(g, v, group, v_group);
  }

  return;
}

// return 1 - no Hamiltonian cycle is present
// return 0 - indeterminate
//
int vcham_cutset_heuristic(vcham_t &g) {
  int i, j, r, k, group, n_group, u_idx;
  int32_t u;
  size_t hist=0;
  std::vector< int32_t > deg_v, v_group;

  // sort by degree, once at the beginning
  //
  hist = g.history.size();
  for (u=0; u<g.n_vertex; u++) {
    deg_v.push_back( g.n_nei[u] );
    deg_v.push_back( u );
  }
  qsort( &(deg_v[0]), g.n_vertex, 2*sizeof(int32_t), cmp_int32_2);

  v_group.resize(g.n_vertex, -1);

  for (i=0, k=1; i<deg_v.size(); i+=2, k++) {

    // init
    //
    for (u=0; u<g.n_vertex; u++) { v_group[u] = -1; }

    // go through and flood fill, using the vertex name as the default
    // group name
    //
    for (u=0; u<g.n_vertex; u++) {
      vcham_ccfill(g, u, u, v_group);
    }

    // count distinct groups
    //
    qsort( &(v_group[0]), g.n_vertex, sizeof(int32_t), cmp_int32);

    group = v_group[0];
    n_group=1;
    for (j=1; j<v_group.size(); j++) {
      if (group != v_group[j]) {
        n_group++;
        group = v_group[j];
      }
    }

    // k starts at 1 and gets incremented.
    // If k+2 ore more distinct groups of verticies
    // exist, by pigeonhole we can't have a cycle.
    //
    if (n_group >= (k+2)) {

      // restore state
      //
      vcham_unwind_history(g, hist);

      return 1;
    }
  }

  // restore state
  //
  vcham_unwind_history(g, hist);

  // could not determine whether no Hamiltonian
  // cycle was present
  //
  return 0;
}

int cmp_edges(vcham_t &g, vcham_t &h) {
  int i, j;
  int32_t u_idx, u, v;

  for (u=0; u<g.n_nei.size(); u++) {
    if (g.n_nei[u] != h.n_nei[u]) {
      fprintf(stderr, "# n_nei mismatch! u%i %i != %i\n",
          u, (int)g.n_nei[u], (int)h.n_nei[u]);
      return 0;
    }
  }

  for (u=0; u<g.edge_idx.size(); u++) {
    u_idx = g.edge_idx[u];
    for (j=0; j<g.n_nei[u]; j++) {
      v = g._edge[u_idx+j];

      if (!vcham_has_edge(h, u, v)) { return 0; }
    }
  }

  for (u=0; u<h.edge_idx.size(); u++) {
    u_idx = h.edge_idx[u];
    for (j=0; j<h.n_nei[u]; j++) {
      v = h._edge[u_idx+j];

      if (!vcham_has_edge(g, u, v)) { return 0; }
    }
  }

  return 1;
}

int vcham_solve_modified_culberson_vandegriend(vcham_t &g, int64_t robin_bound, int64_t runtime_bound) {
  int r;
  int32_t u=0;
  int64_t bound=-1, rt_bound=0;

  vcham_t h;

  g.flag = 0;
  g.toll = 0;

  vcham_copy(g,h);

  if (robin_bound < 0) { robin_bound = (int64_t)g.n_vertex*2; }
  if (runtime_bound < 0) { runtime_bound = (int64_t)( 1LL << 62 ); }

  //DEBUG
  //printf("## cp0 (%i %i) (%i)\n", vcham_check(g), vcham_check(h), cmp_edges(g,h) );

  // test for trivial properties of a graph which does not have a Hamiltonian
  // cycle
  //
  for (u=0; u<g.n_vertex; u++) {
    if (g.n_nei[u]<2) {
      g.flag = NOHAM_TRIVIAL;
      return 0;
    }
  }

  //DEBUG
//  printf("## cp1 (%i %i) (%i)\n",
//      vcham_check(g), vcham_check(h),
//      cmp_edges(g,h) );


  // cut set heuristic
  //
  r = vcham_cutset_heuristic(g);
  if (r==1) {
    g.flag = NOHAM_CUTSET;
    return 0;
  }

  //DEBUG
//  printf("## cp2 (%i %i) (%i)\n",
//      vcham_check(g), vcham_check(h),
//      cmp_edges(g,h) );


  // round robin initial attempt
  //
  for (u=0; u<g.n_vertex; u++) {
    bound = robin_bound;

    g.path.clear();
    memset( &(g.visited[0]), 0, sizeof(unsigned char) * (g.n_vertex) );
    g.path.push_back(u);
    g.visited[u] = 1;

    //DEBUG
    //printf("## cp2.0 (%i %i) (%i)\n", vcham_check(g), vcham_check(h), cmp_edges(g,h) );
    //vcham_debug_print(g);

    r = vcham_solve_r(g, u, robin_bound);

    //DEBUG
    //printf("## cp2.1 (%i %i) (%i)\n", vcham_check(g), vcham_check(h), cmp_edges(g,h) ); 

    if (r<0) {

      vcham_unwind_history(g,0);

      continue;
    }

    g.flag = NOHAM_ROUNDROBIN;
    return r;
  }

  //DEBUG
  //printf("## cp3 (%i %i) (%i)\n", vcham_check(g), vcham_check(h), cmp_edges(g,h) );


  if (g.solve_seed==0) { srand(g.solve_seed); }

  //DEBUG
  //printf("## cp4 (%i %i) (%i)\n", vcham_check(g), vcham_check(h), cmp_edges(g,h) );


  // backtracking search
  // exponential restart
  //
  for (bound = (int64_t)(4*g.n_vertex); bound < runtime_bound; bound *= 2) {
    g.path.clear();
    memset( &(g.visited[0]), 0, sizeof(unsigned char) * (g.n_vertex) );

    u = rand()%g.n_vertex;

    //DEBUG!!
    //u = 36;
    //printf("WARNING, FORCING STARTING VERTEX TO %i\n", u);
    //DEBUG!!

    g.path.push_back(u);
    g.visited[u]=1;


    if (g.verbose) {
      printf("c starting, bound %lli (of %lli)\n", (long long int)bound, (long long int)runtime_bound);
      fflush(stdout);
    }


    //DEBUG
    //printf("## starting with u%i (%i %i) (%i) (bound %lli)\n",
    //    u,
    //    vcham_check(g), vcham_check(h),
    //    cmp_edges(g,h),
    //    (long long int) bound );


    rt_bound = bound;
    r = vcham_solve_r(g, u, rt_bound);

    //DEBUG
    //printf("# bound %lli (of %lli/%lli), r: %i\n",
    //    (long long int)rt_bound,
    //    (long long int)bound,
    //    (long long int)runtime_bound,
    //    (int)r);

    if (r<0) {
      vcham_unwind_history(g,0);
      continue;
    }

    g.flag = HAM_SEARCH;
    g.toll = bound - rt_bound;
    return r;
  }

  //DEBUG
  printf("## TIMEOUT\n");

  g.flag = NOHAM_TIMEOUT;
  return -1;
}
