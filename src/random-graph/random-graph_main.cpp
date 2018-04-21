#include "random-graph.hpp"

#include <getopt.h>
#include <errno.h>

#include <vector>
#include <string>

#define RANDOM_GRAPH_VERSION "0.1.0"

void show_help(FILE *fp) {
  fprintf(fp, "\n");
  fprintf(fp, "random-graph version %s\n", RANDOM_GRAPH_VERSION);
  fprintf(fp, "\n");
  fprintf(fp, "usage:\n");
  fprintf(fp, "\n");
  fprintf(fp, "    random-graph [-n n_vertex] [-m n_edge] [-p edge-prob] [-T type] [-h] [-v] [-V]\n");
  fprintf(fp, "\n");
  fprintf(fp, "  [-n n_vertex]    number of verticies\n");
  fprintf(fp, "  [-m n_edge]      number of edges\n");
  fprintf(fp, "  [-p edge_prob]   edge probability\n");
  fprintf(fp, "  [-T type]        graph type\n");
  fprintf(fp, "  [-h]             help (this screen)\n");
  fprintf(fp, "  [-v]             \n");
  fprintf(fp, "  [-V]             \n");
  fprintf(fp, "\n");

}

int main(int argc, char **argv) {
  int ch;
  int n_v=-1, n_e=-1;
  double p=-1.0;
  int no_opt=1;

  std::string ofn = "-";

  random_graph_t rg;

  while ((ch=getopt(argc, argv, "T:n:p:m:o:hvV"))!=-1) switch (ch) {
    case 'T':
      no_opt=0;
      break;
    case 'n':
      no_opt=0;
      n_v = atoi(optarg);
      break;
    case 'p':
      no_opt=0;
      p = atof(optarg);
      break;
    case 'm':
      no_opt=0;
      n_e = atoi(optarg);
      break;
    case 'v':
      no_opt=0;
      break;
    case 'o':
      ofn = optarg;
      break;
    case 'V':
      no_opt=0;
      break;
    case 'h': show_help(stdout); exit(0); break;
    default:  show_help(stderr); exit(1); break;
  }

  if (no_opt) { show_help(stderr); exit(1); }

  if ((n_v<=0) && (p<=0.0) && (n_e<=0)) {
    fprintf(stderr, "number of verticies, edges and edge probability must be valid\n");
    show_help(stderr);
    exit(1);
  }

  rg_erdos_renyi_G_n_p(rg, n_v, p);

  //rg_fprint_dot(stdout, rg);

  rg_fprint_dimacs(stdout, rg);

}
