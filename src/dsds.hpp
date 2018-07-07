#include <stdio.h>
#include <stdlib.h>

// disjoint set
//


typedef struct disjoint_set_type {
  std::vector< int32_t > parent, rank;
  std::vector< int32_t > history;

  void resize(int n) {
    int i;
    parent.resize(n);
    rank.resize(n);
    for (i=0; i<n; i++) {
      parent[i] = i;
      rank[i] = 0;
    }
  }

  int32_t find(int32_t idx) {
    int32_t fin_idx, t;

    if (parent[idx]==idx) { return idx; }

    for (fin_idx=idx; parent[fin_idx] != fin_idx; fin_idx = parent[fin_idx]);

    while (idx != parent[idx]) {

      history.push_back(0);
      history.push_back(idx);
      history.push_back(parent[idx]);

      t = parent[idx];
      parent[idx] = fin_idx;
      idx = t;
    }

    return fin_idx;
  }

  void set_union(int32_t irep, int32_t jrep) {
    int32_t irank, jrank;

    irep = find(irep);
    jrep = find(jrep);
    irank = rank[irep];
    jrank = rank[jrep];

    if (irep==jrep) { return ; }

    if      (irank < jrank) {

      history.push_back(0);
      history.push_back(irep);
      history.push_back(parent[irep]);

      parent[irep] = jrep;
    }
    else if (irank > jrank) {

      history.push_back(0);
      history.push_back(jrep);
      history.push_back(parent[jrep]);

      parent[jrep] = irep;
    }
    else {

      history.push_back(0);
      history.push_back(irep);
      history.push_back(parent[irep]);

      history.push_back(1);
      history.push_back(jrep);
      history.push_back(rank[jrep]);

      parent[irep] = jrep;
      rank[jrep]++;
    }

  }

  int32_t checkpoint() {
    return (int32_t)(history.size());
  }

  void rewind(int32_t checkpos) {
    int32_t p;

    while ( (int32_t)(history.size()) != checkpos ) {
      val = history.pop_back();
      idx = history.pop_back();
      op  = history.pop_back();

      if      (p==0) { parent[idx] = val; }
      else if (p==1) { rank[idx]   = val; }
    }

  }

} disjoint_set_t;
