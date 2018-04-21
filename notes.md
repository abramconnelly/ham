Random Graph
---

Generate random graphs.
This can be used as a command line tool for debugging or as a library that
can be compiled in.

The PRNG uses `rand()` which hopefully should be good enough for these purposes
but might need to change in the future.
I'm not sure what a suitable replacement is.

TODO:

* Generate different output JSON files for easy viewing in various JavaScript
  graph visualization libraries
* Allow for different distributions
* Allow for minimum and maximum degree (hints)
* Generate graph based on graph degree (link algorithm, Diaconus if we're ambitious)

Viz
---

Visualize graphs.
The purpose is mainly to make sure graph generation is working along with
being able to visually inspect Hamiltonian cycles.

It might be nice to have either an interactive solver that displays what
it's doing along with some simple statistics as it solves.

It might also be nice to have it communicate to a server shim to get
update messages to see an algorithm as it's progressing.

TODO:

* Simple interface to upload a DIMACS graph and visualize it, with an associated
  path or cycle

References:

* [github.com/HelikarLab/ccNetViz](https://github.com/HelikarLab/ccNetViz)


Solver
---

TODO:

* Posa's heuristic
* Vandegriend & Culberson (+) heuristics
* Experiments based on different input graphs


Experiment
---

* VC+ algorithm on 10^{3,4,5,6} for ER random graphs (rg) (param in some reasonable range)
* Posa algorithm on 10^{3,4,5,6} for ER rg (pram in some random range)
  - # iterations (solved, unsolved, total)
  - prob. solveable
* VC+ on 10^{3,4,5,6} for Levy-stable distributions of \alpha \in { 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 }
  for critical parameter (& try to rescale if possible)
  - median & average iterations on solved, unsolved, total
  - prob. solveable
