# kdtree

K-d tree implementation for 2D nearest-neighbour search using Euclidean distance.

## Building

The project uses [Bazel](https://bazel.build/). To build the library and demo binary:

```
bazel build //...
```

Alternatively, build directly with a C++11-compatible compiler:

```
g++ -std=c++11 -o kdtree-demo demo.cc kdtree.cc
```

## Usage

Construct a `Kdtree` from the first point, then `insert` subsequent points. Call
`get_nearby_points` with a query point and a radius to retrieve all points whose
Euclidean distance from the query is at most `radius`.

```cpp
#include "kdtree.h"
#include "point.h"

// Build the tree
Kdtree tree(Point(1, 0.3f, 0.5f));
tree.insert(Point(2, -0.3f, 0.5f));
tree.insert(Point(3,  0.9f, 1.5f));

// Query: find all points within radius 1.0 of (1.3, 0.5)
Point query(10, 1.3f, 0.5f);
std::list<Point> neighbours = tree.get_nearby_points(query, 1.0f);
```

## Running the demo

```
bazel run //:kdtree-demo
```

or, after building manually:

```
./kdtree-demo
```

The demo builds a tree from 100,000 random 2D points, then runs 1,000 range
queries and compares timing and results against a brute-force linear scan.
Expected output (timings will vary):

```
Built k-d tree with 100000 points in ~28 ms
1000 range queries (radius=10):
  K-d tree   : ~6 ms total  (~0.006 ms/query, 7 neighbors avg)
  Brute force: ~100 ms total  (~0.100 ms/query, 7 neighbors avg)
Results match ✓
Speedup: ~16x
```

## Running the tests

```
bazel test //:kdtree-test
```

or, after building manually:

```
g++ -std=c++11 -o kdtree-test test_kdtree.cc kdtree.cc && ./kdtree-test
```
