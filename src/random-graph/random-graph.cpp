#include "random-graph.hpp"

void rg_fprint_dimacs(FILE *fp, random_graph_t &rg) {
  int i;
  fprintf(fp, "c random-graph\n");
  fprintf(fp, "c n %i, m %i, p %f, seed %li, type %i\n",
      rg.n, rg.m, rg.p, rg.seed, rg.type);
  fprintf(fp, "p edge %i %i\n", rg.n, rg.m);
  for (i=0; i<rg.edge.size(); i+=2) {
    fprintf(fp, "e %i %i\n", rg.edge[i]+1, rg.edge[i+1]+1);
  }
}

void rg_fprint_dot(FILE *fp, random_graph_t &rg) {
  int i;

  if (rg.directed) { fprintf(fp, "digraph {\n"); }
  else             { fprintf(fp, "graph {\n"); }

  for (i=0; i<rg.edge.size(); i+=2) {
    fprintf(fp, "  v%i %s v%i\n",
        rg.edge[i], rg.directed ? "->" : "--", rg.edge[i+1]);
  }

  fprintf(fp, "}\n");
}


int rg_erdos_renyi_G_n_p(random_graph_t &rg, uint32_t n, double p) {
  int i, j, k;
  int32_t u, v;
  double d;

  rg.n = n;
  rg.m = 0;
  rg.p = p;
  rg.edge.clear();

  srand48(rg.seed);

  for (u=0; u<n; u++) {

    if (rg.directed) {
      for (v=0; v<u; v++) {
        d = drand48();

        if (d<p) {
          rg.edge.push_back(u);
          rg.edge.push_back(v);
          rg.m++;
        }

      }
    }

    for (v=u+1; v<n; v++) {
      d = drand48();

      if (d<p) {
        rg.edge.push_back(u);
        rg.edge.push_back(v);
        rg.m++;
      }

    }
  }

}
