#ifndef VCHAM_HPP
#define VCHAM_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <vector>
#include <string>

typedef enum {
  HAM_FOUND = 0,
  HAM_SEARCH,
  NOHAM_TRIVIAL,
  NOHAM_CUTSET,
  NOHAM_ROUNDROBIN,
  NOHAM_TIMEOUT,
} noham_flag_enum;

// All verticies that appear here are 0-reference
//
typedef struct vcham_type {
  std::vector< std::string > comment;

  size_t n_vertex, n_edge;

  // "raw" edges flattened out
  // edge_idx points into this structure
  // bp has back pointer information, stored
  //   as index into _edge, of where it's
  //   connected.
  //
  std::vector< int32_t > _edge;
  std::vector< int32_t > _bp;

  // |edge_idx| = n_vertex
  // index into _edge of start position of edge
  // array
  //
  std::vector< int32_t > edge_idx;

  // |n_nei| = n_vertex
  // number of current neightbors for each vertex
  //
  std::vector< size_t > n_nei;
  std::vector< size_t > cap_nei;

  std::vector< int32_t > path;
  std::vector< unsigned char > visited;

  std::vector< int32_t > history;
  size_t history_stride;


  std::string err_msg;

  unsigned int solve_seed;
  int64_t toll;
  int flag;

  vcham_type() : history_stride(6), flag(0), toll(0), solve_seed(0) { }

} vcham_t;

int vcham_init_from_history(vcham_t &g);
int vcham_check(vcham_t &g);
int vcham_read(vcham_t &g, FILE *fp);
void vcham_fprint(FILE *fp, vcham_t &g);
void vcham_fprint_dimacs(FILE *fp, vcham_t &g);
void vcham_fprint_dot_undirected(FILE *fp, vcham_t &g);
void vcham_fprint_dot_undirected_path(FILE *fp, vcham_t &g);

int vcham_reme(vcham_t &g, int32_t u, int32_t v);
int vcham_addback(vcham_t &g);

void vcham_copy(vcham_t &g, vcham_t &h);
int vcham_cmp(vcham_t &g, vcham_t &h);

int vcham_has_raw_edge(vcham_t &g, int32_t u, int32_t v);
int vcham_has_edge(vcham_t &g, int32_t u, int32_t v);

int vcham_cycle_check(vcham_t &g);
int vcham_ham_solve_simple_r(vcham_t &g, int32_t u, int64_t &bound);
int vcham_ham_solve_simple(vcham_t &g);

int vcham_solve_r(vcham_t &g, int32_t u, int64_t &bound);
int vcham_solve(vcham_t &g);
int vcham_solve_modified_culberson_vandegriend(vcham_t &g,  int64_t robin_bound, int64_t runtime_bound);


int vcham_prune_path_vertex(vcham_t &g, int32_t u);
int vcham_prune_between_degree_two(vcham_t &g);

int vcham_unwind_history(vcham_t &g, size_t history_size);

void vcham_ccfill(vcham_t &g, int32_t u, int32_t group, std::vector< int32_t > &v_group);
int vcham_cutset_heuristic(vcham_t &g);

int cmp_int32(const void *a, const void *b);
int cmp_int32_2(const void *a, const void *b);


//---
// tests
//---

int test_rem_raw_edge(vcham_t &g);
int test_rem_single_edges(vcham_t &g);
int test_random_stress(vcham_t &g, int n_it, double p);
int test_random_edges(vcham_t &g);

int test_prune_path_vertex(void);
int test_prune_between_degree_two(void);


void vcham_debugprint(vcham_t &g);

int test_ccfill(void);



#endif
