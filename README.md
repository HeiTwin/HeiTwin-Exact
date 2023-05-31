[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7988134.svg)](https://doi.org/10.5281/zenodo.7988134)

# HeiTwin-Exact
HeiTwin-Exact is an exact solver for the twin-width problem. It takes part in the [PACE Challenge 2023](https://pacechallenge.org/2023/).

## Description
HeiTwin-Exact implements a Branch & Bound algorithm to compute the twin-width of a graph.

First, reductions are applied, i.e. twins are contracted as this does not affect the solution. Then, the Branch & Bound algorithm is executed on each connected component of the graph (or the complement graph, if it has fewer edges).

Different heuristic strategies compute a heuristic solution as an upper for the Branch & Bound algorithm. If a contraction sequence with twin-width 2 is found, it is attempted to find a lower bound of 2 by finding a chordless cycle.

The Branch & Bound algorithm enumerates all possible contractions. To break some symmetry, two independent contractions (i.e. that are pairwise disjoint) are only done in one order.

## Build and run
[argtable3](https://www.argtable.org/) is included as a git submodule. Update it before building:

```git submodule update```

The project can then be build using `cmake`, e.g.

```cmake -Bbuild . && cd build && make```

A binary `HeiTWin` will be created, that reads a graph from stdin and writes the solution to stdout. For more details on the input and output format look at the [PACE Challenge definition](https://pacechallenge.org/2023/io/).

## External libraries
[argtable3](https://www.argtable.org/) is used for command line argument parsing and the [repository](https://github.com/argtable/argtable3.git) is included as a git submodule.
