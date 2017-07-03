// rgr - Random GRaph
//
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <vector>
#include <map>
#include <string>

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

void show_help() {
  printf("rgr - Random GRaph generation in DIMACS format\n");
  printf("usage:\n");
  printf("\n");
  printf("    rgr [-h] [-v] [-e edge] [-v vertices] [-p prob] [-S seed] [-C comment]\n");
  printf("\n");
  printf("  [-p prob]       edge probability)\n");
  printf("  [-v vertices]   number of verticies)\n");
  printf("  [-e edge]       number of edges\n");
  printf("  [-S seed]       seed for rng\n");
  printf("  [-C comment]    comment\n");
  printf("  [-f ofmt]       output format (dimcas, json, cstr)\n");
  printf("  [-U]            undirected graph\n");
  printf("  [-L]            pair loop edges (from->to, to->from)\n");
  printf("  [-M]            multi edge graph (don't force maximum single edge between nodes)\n");
  printf("  [-v]            version\n");
  printf("  [-h]            help (this screen)\n");
}

void output_json(std::string &comment, int n_nodes, int n_edges, unsigned int seed, std::vector< long int > &edge_v) {
  long int i, r0, r1;
  return;

  printf("{\n");
  printf("  \"nodes\":[\n");
  printf("    { source:");
  printf("  ],\n");
  printf("  \"edges\":[\n");
  printf("  ]\n");
  printf("}\n");
}


void output_str(std::string &comment, int n_nodes, int n_edges, unsigned int seed, std::vector< long int > &edge_v) {
  long int i, r0, r1;

  if (comment.size()==0) {
    printf("c rgr n%i, e%i, seed %u\\n", n_nodes, n_edges, seed);
  } else {
    printf("c %s\\n", comment.c_str());
  }
  printf("p edge %i %i\\n", n_nodes, n_edges);

  for (i=0; i<edge_v.size(); i+=2) {
    r0=edge_v[i]+1;
    r1=edge_v[i+1]+1;

    printf("e %li %li\\n", r0, r1);

  }

}

void output_dimacs(std::string &comment, int n_nodes, int n_edges, unsigned int seed, std::vector< long int > &edge_v) {
  long int i, r0, r1;

  if (comment.size()==0) {
    printf("c rgr n%i, e%i, seed %u\n", n_nodes, n_edges, seed);
  } else {
    printf("c %s\n", comment.c_str());
  }
  printf("p edge %i %i\n", n_nodes, n_edges);

  for (i=0; i<edge_v.size(); i+=2) {
    r0=edge_v[i]+1;
    r1=edge_v[i+1]+1;

    printf("e %li %li\n", r0, r1);

  }

}


int main(int argc, char **argv) {
  int i, j, k;
  int n_nodes=-1, n_edges=-1;
  unsigned int seed=0;
  long int r0, r1, tr;
  long int exclude[4];
  std::string comment, out_format;
  double prob = -1;
  int undirected_flag = 0, loop_flag = 0, multi_edge=0;

  std::pair<int, int> key;
  std::map< std::pair<int, int>, int> edge_flag;
  std::vector< long int > edge_v;

  int ch;

  out_format = "dimacs";

  while ((ch=getopt(argc, argv, "he:v:p:S:C:F:ULM"))!=-1) switch(ch) {
    case 'p': prob = atof(optarg); break;
    case 'v': n_nodes = atoi(optarg); break;
    case 'e': n_edges = atoi(optarg); break;
    case 'S': seed = (unsigned int)atoi(optarg); break;
    case 'C': comment = optarg; break;
    case 'U': undirected_flag = 1; break;
    case 'L': loop_flag = 1; break;
    case 'M': multi_edge = 1 ; break;
    case 'F': out_format = optarg; break;
    case 'h':
    default:
      show_help();
      exit(0);
      break;
  }

  if ((prob>=0.0) && (prob<=1.0)) {
    n_edges = (double)(n_nodes*(n_nodes-1)/2);
    n_edges = (int)((double)n_edges * prob);
  }

  if ((n_edges<0) || (n_nodes<0)) {
    fprintf(stderr, "edges and verticies must be greater than 0\n");
    show_help();
    exit(1);
  }

  srand(seed);

  /*
  if (comment.size()==0) {
    printf("c rgr n%i, e%i, seed %u\n", n_nodes, n_edges, seed);
  } else {
    printf("c %s\n", comment.c_str());
  }
  printf("p edge %i %i\n", n_nodes, n_edges);
  */

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

  if (out_format == "dimacs") {
    output_dimacs(comment, n_nodes, n_edges, seed, edge_v);
  }
  else if (out_format == "json") {
    output_json(comment, n_nodes, n_edges, seed, edge_v);
  }
  else if (out_format == "str") {
    output_str(comment, n_nodes, n_edges, seed, edge_v);
  }


}
