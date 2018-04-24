#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <vector>
#include <string>

#define EXE_NAME "er"
#define EXE_VERSION "0.1.0"

void show_version(FILE *fp) {
  fprintf(fp, "%s version: %s\n", EXE_NAME, EXE_VERSION);
}

void show_help(FILE *fp) {
  show_version(fp);
  fprintf(fp, "\n");
  fprintf(fp, "usage:\n");
  fprintf(fp, "\n");
  fprintf(fp, "    %s [-h] [-v] [-V] [-n n] [-p p] [-s seed] [-m m] [-D d] [-B]:\n", EXE_NAME);
  fprintf(fp, "\n");

  fprintf(fp, "\n");
  fprintf(fp, "\n");
  fprintf(fp, "\n");
}

int fill_mindeg(std::vector< std::vector< int > > &ve, int min_deg) {
  int i, j, k, t;
  int u, v, n_sample;
  size_t n_v, n;
  std::vector< int > vv;

  n_v = ve.size();
  vv.resize(n_v);

  for (u=0; u<n_v; u++) {

    if (ve[u].size() >= min_deg) { continue; }

    for (i=0; i<n_v; i++) { vv[i]=i; }

    // put neighbors at end of list
    //
    for (i=0; i<ve[u].size(); i++) {
      v = ve[u][i];
      t = vv[v];
      vv[v] = vv[v_n-1-i];
      vv[v_n-1-i] = t;
    }

    // don't inculde this vertex
    //
    t = vv[u];
    vv[u] = vv[v_n-1-i];
    vv[v_n-1-i] = t;

    n = min_deg - ve[i].size();
    for (j=0; j<n; j++) {
    }

  }

}

int main(int argc, char **argv) {
  int i, j, k, n_v, n_e=0, min_deg=0;
  int seed=0;
  double p=-1, c_param = 0.0;;
  std::vector< std::vector< int > > ve;
  std::vector< int > empty_v;

  int verbose_flag = 0, bipartite_flag = 0;
  int dot_format=0;

  int opt;

  while ((opt=getopt(argc, argv, "hvVn:p:m:D:s:BT"))>=0) switch(opt) {
    case 'h':
      show_help(stdout);
      exit(0);
    case 'v':
      verbose_flag = 1;
      break;
    case 'V':
      show_version(stdout);
      exit(0);

    case 's':
      seed = atoi(optarg);
      break;
    case 'p':
      p = atof(optarg);
      break;
    case 'm':
      n_e = atoi(optarg);
      break;
    case 'B':
      bipartite_flag = 1;
      break;
    case 'D':
      min_deg = atoi(optarg);
      break;
    case 'n':
      n_v = atoi(optarg);
      break;
    case 'c':
      c_param = atof(optarg);
      break;

    case 'T':
      dot_format = 1;
      break;

    default:
      show_help(stderr);
      exit(-1);
  }

  srand48(seed);

  for (i=0; i<n_v; i++) { ve.push_back(empty_v); }

  for (i=0; i<n_v; i++) {
    for (j=i+1; j<n_v; j++) {
      if (i==j) { continue; }
      if (drand48() < p) {
        ve[i].push_back(j);
        n_e++;
      }
    }
  }

  if (min_deg > 0) {
    fill_min_deg(ve, min_deg);
  }

  if (dot_format) {
    printf("graph {\n");
    for (i=0; i<n_v; i++) {
      for (j=0; j<ve[i].size(); j++) {
        printf("  v%i -- v%i;\n", i+1, ve[i][j]+1);
      }
    }
    printf("}\n");
    exit(0);
  }

  printf("c random graph\n");
  printf("c seed %i, p %f, n %i, m %i, min_deg %i\n", seed, p, n_v, n_e, min_deg);
  printf("p edge %i %i\n", n_v, n_e);
  for (i=0; i<n_v; i++) {
    for (j=0; j<ve[i].size(); j++) {
      printf("e %i %i\n", i+1, ve[i][j]+1);
    }
  }
}
