#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "vcham.hpp"

#define VCHAM_VERSION "0.1.0"

int run_tests(vcham_t &g) {
  int r, verbose=1;
  vcham_t h;

  vcham_copy(g, h);

  r = test_rem_single_edges(g);

  if (verbose) {
    printf("# single edges test: %i\n", r);
    printf("# cmp: %i\n", vcham_cmp(g,h));
  }


  r = test_random_stress(g, 10, 0.5);
  if (verbose) {
    printf("## stress test >>> %i (check %i))\n",
        r, vcham_check(g));
  }

  r = test_rem_raw_edge(g);
  if (verbose) {
    printf("## test_rem_raw_edge: %i\n", r);
  }

  test_random_edges(g);

  if (verbose) {
    printf("### check: %i\n", vcham_check(g));
  }

}

void show_version(FILE *fp) {
  fprintf(fp, "vcham version: %s\n", VCHAM_VERSION);
}

void show_help(FILE *fp) {
  show_version(fp);
  fprintf(fp, "\n");
  fprintf(fp, "usage:\n");
  fprintf(fp, "\n");
  fprintf(fp, "  vcham [-h] [-v] [-V] [-B] [-P] [-i ifn]\n");
  fprintf(fp, "\n");
  fprintf(fp, "    [-i ifn] input DIMACS file\n");
  fprintf(fp, "    [-B]     brute force\n");
  fprintf(fp, "    [-P]     print dimacs graph\n");
  fprintf(fp, "    [-V]     show version\n");
  fprintf(fp, "    [-h]     help (this screen)\n");
  fprintf(fp, "\n");
}

int main(int argc, char **argv) {
  int u, v;
  int i, r;
  int opt;
  FILE *fp = stdin;
  std::string ifn;
  vcham_t g, h;

  int brute_force = 0, print_dimacs = 0, verbose_flag = 0;

  int64_t bound=-1, max_bound=-1;

  FILE *ofp = stdout;

  while ((opt=getopt(argc, argv, "i:hvVBPb:"))!=-1) switch(opt) {

    case 'i':
      ifn = optarg;
      break;

    case 'b':
      max_bound = strtoll(optarg, NULL, 10);
      break;

    case 'B':
      brute_force = 1;
      break;

    case 'P':
      print_dimacs = 1;
      break;
    case 'v':
      verbose_flag = 1;
      break;
    case 'V':
      show_version(stdout);
      exit(0);
      break;
    case 'h':
      show_help(stdout);
      exit(0);
      break;

    default:
      show_help(stderr);
      exit(-1);
      break;
  }

  if (optind < argc) {
    ifn = argv[optind];
  }

  if (ifn=="") {
    show_help(stderr);
    exit(-1);
  }

  if (ifn != "-") {
    fp = fopen(ifn.c_str(), "r");
    if (!fp) {
      perror(ifn.c_str());
      exit(-1);
    }
  }

  r = vcham_read(g, fp);
  if (r!=0) {
    fprintf(stderr, "error, got %i\n", r);
    exit(-1);
  }

  bound = max_bound;
  g.path.push_back(0);
  g.visited[0] = 1;
  r = vcham_solve_r(g, 0, bound);

  if (max_bound < 0) {
    bound = -bound;
  }
  else {
    bound = max_bound - bound;
  }

  if (r<0) {
    printf("TIMEOUT\n");
  }
  else if (r==1) {
    printf("1 CYCLE %i\n", (int)(bound));
  }
  else {
    printf("0 NON %i\n", (int)(bound));
  }

  if (r==1) {
    if (vcham_cycle_check(g))  {
      if (verbose_flag) { printf("# check: ok\n"); }
    }
    else {
      printf("# ERROR, path check failed!\n");
    }

  }

  if (verbose_flag) {
    if (r==1) {
      for (i=0; i<g.path.size(); i++) {
        printf(" %i", (int)g.path[i]);
      }
      printf("\n");
    }
  }

  if (print_dimacs) {
    if (r) {
      vcham_fprint_dot_undirected_path(stdout, g);
    }
    else {
      vcham_fprint_dot_undirected(stdout, g);
    }
  }

}
