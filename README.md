# kdtree

N-dimensional k-d tree implementation for nearest-neighbour search using Euclidean distance.

Supports any number of dimensions (2D, 3D, 4D, etc.) through C++ templates with zero runtime overhead.

## Building

The project uses [Bazel](https://bazel.build/). To build the library and demo binary:

```
bazel build //...
```

Alternatively, build directly with a C++11-compatible compiler:

```
g++ -std=c++11 -o kdtree-demo demo.cc
```

## Usage

The library uses C++ templates to support any number of dimensions. Use `Point<N>` and `Kdtree<N>` where `N` is the number of dimensions.

### 2D Example

```cpp
#include "kdtree.h"
#include "point.h"

// Type aliases for convenience
using Point2D = Point<2>;
using Kdtree2D = Kdtree<2>;

// Build the tree
Kdtree2D tree(Point2D(1, {{0.3f, 0.5f}}));
tree.insert(Point2D(2, {{-0.3f, 0.5f}}));
tree.insert(Point2D(3, {{0.9f, 1.5f}}));

// Query: find all points within radius 1.0 of (1.3, 0.5)
Point2D query(10, {{1.3f, 0.5f}});
std::list<Point2D> neighbours = tree.get_nearby_points(query, 1.0f);
```

### 3D Example

```cpp
#include "kdtree.h"
#include "point.h"

using Point3D = Point<3>;
using Kdtree3D = Kdtree<3>;

// Build the tree with 3D points
Kdtree3D tree(Point3D(1, {{0.3f, 0.5f, 0.2f}}));
tree.insert(Point3D(2, {{-0.3f, 0.5f, 0.1f}}));
tree.insert(Point3D(3, {{0.9f, 1.5f, 0.8f}}));

// Query in 3D space
Point3D query(10, {{1.3f, 0.5f, 0.2f}});
std::list<Point3D> neighbours = tree.get_nearby_points(query, 1.0f);
```

### Accessing Coordinates

Points support both generic indexing and named accessors:

```cpp
Point3D p(1, {{1.0f, 2.0f, 3.0f}});

// Generic indexing (works for any dimension)
float x = p[0];
float y = p[1];
float z = p[2];

// Named accessors (for convenience)
float x = p.x();  // First dimension
float y = p.y();  // Second dimension
float z = p.z();  // Third dimension
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
g++ -std=c++11 -o kdtree-test test_kdtree.cc && ./kdtree-test
```

Tests include both 2D and 3D test cases to verify the generalization works correctly.

## Performance

K-d trees work efficiently for low to moderate dimensionality:
- **2D-3D**: Excellent performance, significant speedup over brute force
- **4D-10D**: Good performance, still much faster than linear search
- **10D+**: Performance degrades due to the "curse of dimensionality"

For very high-dimensional spaces (20+ dimensions), consider alternative data structures.

## Implementation Notes

This is a header-only library (templates require definition in headers). The implementation uses:
- `std::array<float, N>` for compile-time dimensionality with zero overhead
- Generic axis cycling: `(split_axis + 1) % N`
- N-dimensional Euclidean distance in range queries
- Efficient pruning based on hyperplane intersections
