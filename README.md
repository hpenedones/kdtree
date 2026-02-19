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
