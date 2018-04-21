#include "vcham.hpp"

void vcham_fprint_dot_undirected(FILE *fp, vcham_t &g) {
  int u, v;
  int idx, idx_offset;
  fprintf(fp, "graph {\n");
  for (u=0; u<g.n_vertex; u++) {
    idx = g.edge_idx[u];
    for (idx_offset=0; idx_offset<g.n_nei[u]; idx_offset++) {
      v = g._edge[idx + idx_offset];
      if (v < u) { continue; }
      fprintf(fp, "  v%i -- v%i;\n", u, v);
    }
  }
  fprintf(fp, "}\n");
}

void vcham_fprint_dot_undirected_path(FILE *fp, vcham_t &g) {
  int i, u, v;
  int idx, idx_offset;
  fprintf(fp, "graph {\n");
  for (u=0; u<g.n_vertex; u++) {
    idx = g.edge_idx[u];
    for (idx_offset=0; idx_offset<g.cap_nei[u]; idx_offset++) {
      v = g._edge[idx + idx_offset];
      if (v < u) { continue; }
      fprintf(fp, "  v%i -- v%i;\n", u, v);
    }
  }

  if (g.path.size()>2) {
    for (i=1; i<g.path.size(); i++) {
      fprintf(fp, "  v%i -- v%i [width=4, color=\"#ff8888\"];\n", g.path[i-1], g.path[i]);
    }
    fprintf(fp, "  v%i -- v%i [width=4, color=\"#ff8888\"];\n", g.path[0], g.path[g.path.size()-1]);
  }

  fprintf(fp, "}\n");
}

void vcham_debugprint(vcham_t &g) {
  int i, j, k, u, v, w, x, n;
  int idx, u_idx, v_idx, u_offset, v_offset;

  for (u=0; u<g.n_vertex; u++) {
    n = g.n_nei[u];
    idx = g.edge_idx[u];

    printf("v%i:", u);
    for (v_offset=0; v_offset<n; v_offset++) {
      printf(" v%i:bp(%i),idx(%i)",
          (int)g._edge[idx + v_offset],
          (int)g._bp[idx + v_offset],
          (int)(idx+v_offset));
    }
    printf("\n");

  }
}

void vcham_copy(vcham_t &g, vcham_t &h) {
  h.n_vertex = g.n_vertex;
  h.n_edge = g.n_edge;
  h.comment = g.comment;
  h._edge = g._edge;
  h._bp = g._bp;
  h.edge_idx = g.edge_idx;
  h.n_nei = g.n_nei;
  h.cap_nei = g.cap_nei;
  h.history = g.history;
}

int vcham_cmp(vcham_t &g, vcham_t &h) {
  int i;

  if (h.n_vertex != g.n_vertex) { return -1; }
  if (h.n_edge != g.n_edge) { return -2; }

  if (h._edge.size() != g._edge.size()) { return -3; }
  for (i=0; i<h._edge.size(); i++) {
    if (h._edge[i] != g._edge[i]) { return -4; }
  }

  if (h._bp.size() != g._bp.size()) { return -5; }
  for (i=0; i<h._bp.size(); i++) {
    if (h._bp[i] != g._bp[i]) { return -6; }
  }

  if (h.edge_idx.size() != g.edge_idx.size()) { return -7; }
  for (i=0; i<h.edge_idx.size(); i++) {
    if (h.edge_idx[i] != g.edge_idx[i]) { return -8; }
  }

  if (h.history.size() != g.history.size()) { return -9; }
  for (i=0; i<h.history.size(); i++) {
    if (h.history[i] != g.history[i]) { return -10; }
  }

  if (h.n_nei.size() != g.n_nei.size()) { return -11; }
  for (i=0; i<h.n_nei.size(); i++) {
    if (h.n_nei[i] !=  g.n_nei[i]) { return -12; }
  }

  return 0;
}

// remove an edge
// * find each edge in the _edge list
// * swap them for the current end
// * update back pointers (_bp)
// * add to history
//
int vcham_reme(vcham_t &g, int32_t u, int32_t v) {
  int i, r;
  size_t n, m;
  int32_t u_idx, v_idx, u_offset, v_offset;
  int32_t x_pos, y_pos;
  int32_t t;

  // find v in u
  //
  u_idx = g.edge_idx[u];
  n = g.n_nei[u];
  for (v_offset=0; v_offset<n; v_offset++) {
    if (g._edge[u_idx + v_offset] == v) { break; }
  }
  if (v_offset==n) { return -1; }

  // find u in v
  //
  v_idx = g.edge_idx[v];
  m = g.n_nei[v];
  for (u_offset=0; u_offset<m; u_offset++) {
    if (g._edge[v_idx + u_offset] == u) { break; }
  }
  if (u_offset==m) { return -1; }

  // swap entries
  //
  t = g._edge[u_idx + v_offset];
  g._edge[u_idx + v_offset] = g._edge[u_idx + n - 1];
  g._edge[u_idx + n - 1] = t;

  t = g._bp[u_idx + v_offset];
  g._bp[u_idx + v_offset] = g._bp[u_idx + n - 1];
  g._bp[u_idx + n - 1] = t;

  //--

  t = g._edge[v_idx + u_offset];
  g._edge[v_idx + u_offset] = g._edge[v_idx + m - 1];
  g._edge[v_idx + m - 1] = t;

  t = g._bp[v_idx + u_offset];
  g._bp[v_idx + u_offset] = g._bp[v_idx + m - 1];
  g._bp[v_idx + m - 1] = t;

  // update back pointers
  //
  g._bp[u_idx + n - 1] = v_idx + m - 1;
  g._bp[v_idx + m - 1] = u_idx + n - 1;

  x_pos = u_idx + v_offset;
  y_pos = v_idx + u_offset;

  g._bp[ g._bp[x_pos] ] = x_pos;
  g._bp[ g._bp[y_pos] ] = y_pos;

  g.n_nei[u]--;
  g.n_nei[v]--;

  g.history.push_back(u);
  g.history.push_back(x_pos);
  g.history.push_back(u_idx+n-1);

  g.history.push_back(v);
  g.history.push_back(y_pos);
  g.history.push_back(v_idx+m-1);

  return 0;
}

// 'undo' the edge removal from vcham_reme
// peels off history elements and patches
// up _ebp
//
int vcham_addback(vcham_t &g) {
  size_t n;
  int32_t u, v, u_idx, v_idx, u_end_idx, v_end_idx;
  int32_t t;

  n = g.history.size();
  if (n == 0)   { return -1; }
  if ((n%6)!=0) { return -2; }

  v_end_idx = g.history[n-1];
  v_idx     = g.history[n-2];
  v         = g.history[n-3];

  u_end_idx = g.history[n-4];
  u_idx     = g.history[n-5];
  u         = g.history[n-6];

  g.history.resize(n-6);

  // swap entries
  //
  t = g._edge[u_idx];
  g._edge[u_idx] = g._edge[u_end_idx];
  g._edge[u_end_idx] = t;

  t = g._bp[u_idx];
  g._bp[u_idx] = g._bp[u_end_idx];
  g._bp[u_end_idx] = t;

  //--

  t = g._edge[v_idx];
  g._edge[v_idx] = g._edge[v_end_idx];
  g._edge[v_end_idx] = t;

  t = g._bp[v_idx];
  g._bp[v_idx] = g._bp[v_end_idx];
  g._bp[v_end_idx] = t;

  //--

  g._bp[ u_idx ] = v_idx;
  g._bp[ v_idx ] = u_idx;

  g._bp[ g._bp[ u_end_idx ] ] = u_end_idx;
  g._bp[ g._bp[ v_end_idx ] ] = v_end_idx;

  g.n_nei[u]++;
  g.n_nei[v]++;

  return 0;
}

int vcham_check(vcham_t &g) {
  int i, j, k, err_code=-1;
  size_t n_edge, n_vertex, s=0, n, n_e_hist, stride;
  int32_t u, v, u_idx;

  n_vertex = g.n_vertex;
  n_edge = g.n_edge;
  stride = g.history_stride;

  if ((g.history.size()%stride) != 0) { return err_code; }
  err_code--;

  n_e_hist = g.history.size()/stride;

  if (n_vertex != g.n_nei.size()) { return err_code; }
  err_code--;

  if (n_vertex != g.cap_nei.size()) { return err_code; }
  err_code--;

  if (n_vertex != g.edge_idx.size()) { return err_code; }
  err_code--;

  if (g._edge.size() != g._bp.size()) { return err_code; }
  err_code--;

  for (s=0, i=0; i<g.n_nei.size(); i++) {
    s += g.n_nei[i];
  }
  if ((s + 2*n_e_hist)!=(2*n_edge)) { return err_code; }
  err_code--;

  for (s=0, i=0; i<g.cap_nei.size(); i++) {
    s += g.cap_nei[i];
  }
  if (s != (2*n_edge)) { return err_code; }
  err_code--;

  //--

  for (u=0; u<g.n_vertex; u++) {
    u_idx = g.edge_idx[u];
    for (j=0; j<g.n_nei[u]; j++) {
      if (u != g._edge[ g._bp[u_idx + j] ]) { return err_code; }
    }
  }
  err_code--;

  for (u=0; u<g.n_vertex; u++) {
    u_idx = g.edge_idx[u];
    for (j=0; j<g.n_nei[u]; j++) {
      if (g._bp[ g._bp[u_idx + j] ] != (u_idx + j)) {
        return err_code;
      }
    }
  }
  err_code--;

  //--

  for (u=0; u<g.n_vertex; u++) {
    u_idx = g.edge_idx[u];
    for (j=0; j<g.cap_nei[u]; j++) {
      if (u != g._edge[ g._bp[u_idx + j] ]) { return err_code; }
    }
  }
  err_code--;

  for (u=0; u<g.n_vertex; u++) {
    u_idx = g.edge_idx[u];
    for (j=0; j<g.cap_nei[u]; j++) {
      if (g._bp[ g._bp[u_idx + j] ] != (u_idx + j)) {
        return err_code;
      }
    }
  }
  err_code--;

  //--

  for (u=0; u<g.n_vertex; u++) {
    n = ( ((u+1) < g.n_vertex) ?  (g.edge_idx[u+1] - g.edge_idx[u]) : (g._edge.size() - g.edge_idx[u]) );
    u_idx = g.edge_idx[u];
    for (j=0; j<n; j++) {
      if (u != g._edge[ g._bp[u_idx + j] ]) { return err_code; }
    }
  }
  err_code--;

  for (u=0; u<g.n_vertex; u++) {
    n = ( ((u+1) < g.n_vertex) ?  (g.edge_idx[u+1] - g.edge_idx[u]) : (g._edge.size() - g.edge_idx[u]) );
    u_idx = g.edge_idx[u];
    for (j=0; j<n; j++) {
      if (g._bp[ g._bp[u_idx + j] ] != (u_idx + j)) {
        return err_code;
      }
    }
  }
  err_code--;

  return 0;
}

void vcham_fprint_dimacs(FILE *fp, vcham_t &g) {
  int i, j, idx;

  for (i=0; i<g.comment.size(); i++) {
    fprintf(fp, "%s\n", g.comment[i].c_str());
  }

  fprintf(fp, "p edge %i %i\n", (int)g.n_vertex, (int)g.n_edge);
  for (i=0; i<g.edge_idx.size(); i++) {
    for (j=0; j<g.n_nei[i]; j++) {
      idx = g.edge_idx[i];
      if (g._edge[idx+j] < i) { continue; }
      fprintf(fp, "e %i %i\n", i+1, g._edge[idx+j]+1);
    }
  }

}


static int _parse_problem_line(std::string &s, int32_t &v, int32_t &e) {
  size_t n;
  int idx=0;
  char *chp;
  long long int lli;

  n = s.size();

  for (idx=0; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  if (s[idx] != 'p') { return -2; }
  for (idx=idx+1; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  if (strncmp(s.c_str() + idx, "edge", 4)!=0) { return -3; }
  idx+=4;
  for (idx=idx+1; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  lli = strtoll(s.c_str() + idx, NULL, 10);
  if ((errno==EINVAL) || (errno==ERANGE)) { return -4; }
  v = (int32_t)lli;

  for (idx=idx+1; (idx<n) && (s[idx]!=' '); idx++);
  if (idx==n) { return -1; }
  for (idx=idx+1; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  lli = strtoll(s.c_str() + idx, NULL, 10);
  if ((errno==EINVAL) || (errno==ERANGE)) { return -4; }
  e = (int32_t)lli;

  return 0;
}

static int _parse_edge_line(std::string &s, int32_t &x, int32_t &y) {
  size_t n;
  int idx=0;
  char *chp;
  long long int lli;

  n = s.size();

  for (idx=0; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  if (s[idx] != 'e') { return -2; }
  for (idx=idx+1; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  lli = strtoll(s.c_str() + idx, NULL, 10);
  if ((errno==EINVAL) || (errno==ERANGE)) { return -4; }
  x = (int32_t)lli;

  for (idx=idx+1; (idx<n) && (s[idx]!=' '); idx++);
  if (idx==n) { return -1; }
  for (idx=idx+1; (idx<n) && (s[idx]==' '); idx++);
  if (idx==n) { return -1; }

  lli = strtoll(s.c_str() + idx, NULL, 10);
  if ((errno==EINVAL) || (errno==ERANGE)) { return -4; }
  y = (int32_t)lli;

  return 0;
}

int edge_cmp(const void *a, const void *b) {
  int32_t *x, *y;
  x = (int32_t *)a;
  y = (int32_t *)b;

  if (x[0] < y[0]) { return -1; }
  if (x[0] > y[0]) { return  1; }
  if (x[1] < y[1]) { return -1; }
  if (x[1] > y[1]) { return  1; }
  return 0;
}

int vcham_init_from_history(vcham_t &g) {
  size_t n, n_nei=0, sum=0;
  int32_t cur_v=0;
  int i, j, prev_start=0, idx;

  int32_t from_idx, to_idx;
  int32_t u, v, v_base_idx, u_base_idx;
  int32_t u_in_v_offset;

  g.visited.resize( g.n_vertex );
  for (i=0; i<g.n_vertex; i++) { g.visited[0]; }

  n = g.history.size();

  qsort(&(g.history[0]), 2*g.n_edge, 2*sizeof(int32_t), edge_cmp);

  // setup _edge, edge_idx and n_nei arrays
  //
  for (i=0; i<n; i+=2) {
    while (cur_v < g.history[i]) {
      g.edge_idx.push_back((int32_t)g._edge.size());
      g.n_nei.push_back(n_nei);
      for (j=prev_start; j<i; j+=2) {
        g._edge.push_back(g.history[j+1]);
      }
      cur_v++;
      n_nei=0;
      prev_start = i;
    }
    n_nei++;
  }
  while (cur_v < g.n_vertex) {
    g.edge_idx.push_back((int32_t)g._edge.size());
    g.n_nei.push_back(n_nei);
    for (j=prev_start; j<n; j+=2) {
      g._edge.push_back(g.history[j+1]);
    }
    cur_v++;
    n_nei=0;
    prev_start = n;
  }

  // calculate back pointer
  // each vertex in the sub array in _edge
  // is sorted so we can bsearch if the array
  // is too large to linearly search through
  //
  // we know the size of _bp, so preallocate
  //
  g._bp.resize(g._edge.size());
  for (i=0; i<g.n_vertex; i++) {
    u = i;
    u_base_idx = g.edge_idx[i];
    for (j=0; j<g.n_nei[i]; j++) {

      v = g._edge[u_base_idx+j];

      if (v>=g.edge_idx.size()) {
        printf("SANITY ERROR: v%i > edge_idx %i\n", (int)v, (int)g.edge_idx.size());
        exit(-3);
      }

      v_base_idx = g.edge_idx[v];

      for (u_in_v_offset=0; u_in_v_offset<g.n_nei[v]; u_in_v_offset++) {
        if (g._edge[v_base_idx + u_in_v_offset] == u) { break; }
      }
      if (u_in_v_offset==g.n_nei[v]) { return -1; }

      g._bp[u_base_idx + j] = v_base_idx + u_in_v_offset;
      g._bp[v_base_idx + u_in_v_offset] = u_base_idx + j;

    }
  }

  // keep a copy of the original neighbor count
  //
  g.cap_nei = g.n_nei;

  g.history.clear();

  return 0;
}

int vcham_has_edge(vcham_t &g, int32_t u, int32_t v) {
  int i, u_idx;

  u_idx = g.edge_idx[u];
  for (i=0; i<g.n_nei[u]; i++) {
    if (g._edge[u_idx+i] == v) { return 1; }
  }

  return 0;
}

int vcham_has_raw_edge(vcham_t &g, int32_t u, int32_t v) {
  int i, idx, s;
  size_t n;

  idx = g.edge_idx[u];
  n = (((u+1)==g.n_vertex) ? (g._edge.size() - g.edge_idx[u]) : (g.edge_idx[u+1] - g.edge_idx[u]));
  for (i=0; i<n; i++) {
    if (g._edge[idx+i] == v) { break; }
  }
  if (i==n) { return 0; }

  idx = g.edge_idx[v];
  n = (((v+1)==g.n_vertex) ? (g._edge.size() - g.edge_idx[v]) : (g.edge_idx[v+1] - g.edge_idx[v]));
  for (i=0; i<n; i++) {
    if (g._edge[idx+i] == u) { break; }
  }
  if (i==n) { return 0; }

  return 1;
}

int vcham_read(vcham_t &g, FILE *fp) {
  int i, j, k;
  std::string buf;
  char *saveptr, *token;
  int ch, r=0;

  int32_t n_vertex, n_edge;
  int32_t x, y;

  g._edge.clear();
  g._bp.clear();
  g.edge_idx.clear();
  g.n_nei.clear();
  g.history.clear();

  g.path.clear();
  g.visited.clear();

  while (!feof(fp)) {
    ch = fgetc(fp);
    if ((ch==EOF) || (ch=='\n')) {

      if (buf.size()==0) { continue; }

      // comment
      //
      if (buf[0]=='c') {
        g.comment.push_back(buf);
      }

      // problem line
      //
      else if (buf[0] == 'p') {

        r = _parse_problem_line(buf, n_vertex, n_edge);
        if (r<0) { return -1; }

        g.n_vertex = (size_t)n_vertex;
        g.n_edge = (size_t)n_edge;

        g._edge.reserve(2*n_edge);
        g._bp.reserve(2*n_edge);
        g.n_nei.reserve(n_vertex);
        g.edge_idx.reserve(n_vertex);

        g.history.reserve(4*n_edge);
      }

      // edge line
      //
      else if (buf[0] == 'e') {

        r = _parse_edge_line(buf, x, y);

        g.history.push_back(x-1);
        g.history.push_back(y-1);
        g.history.push_back(y-1);
        g.history.push_back(x-1);
      }

      buf.clear();
      continue;
    }

    buf += (char)ch;
  }

  r = vcham_init_from_history(g);
  if (r<0) { return r; }

  return 0;
}

void vcham_fprint(FILE *fp, vcham_t &g) {
}
