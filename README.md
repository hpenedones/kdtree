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
float x_coord = p[0];
float y_coord = p[1];
float z_coord = p[2];

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

| Dimension | Query Complexity | Practical Use |
|-----------|------------------|---------------|
| 2D | O(√n + k) | Excellent ✓ |
| 3D | O(n^(2/3) + k) | Excellent ✓ |
| 4D-5D | O(n^(3/4) + k) | Very good ✓ |
| 6D-10D | O(n^0.85 + k) | Good ✓ |
| 20D+ | O(n) | Poor (use other structures) |

For very high-dimensional spaces (20+ dimensions), consider alternative data structures due to the "curse of dimensionality".

## Implementation Notes

This is a header-only library (templates require definition in headers). The implementation uses:
- `std::array<float, N>` for compile-time dimensionality with zero overhead
- Generic axis cycling: `(split_axis + 1) % N`
- N-dimensional Euclidean distance in range queries
- Efficient pruning based on hyperplane intersections

## Python Wrapper

A Python wrapper is available using pybind11. It supports 2D, 3D, and 4D k-d trees.

### Building the Python Extension

```bash
# Install pybind11
pip install pybind11

# Build and install the extension
python setup.py install
```

### Python Usage

```python
import pykdtree

# Create 2D points
p1 = pykdtree.Point2D(1, [0.3, 0.5])
p2 = pykdtree.Point2D(2, [-0.3, 0.5])
p3 = pykdtree.Point2D(3, [0.9, 1.5])

# Build tree
tree = pykdtree.Kdtree2D(p1)
tree.insert(p2)
tree.insert(p3)

# Query: find all points within radius 1.0 of (1.3, 0.5)
query = pykdtree.Point2D(10, [1.3, 0.5])
neighbors = tree.get_nearby_points(query, 1.0)

print(f"Found {len(neighbors)} neighbors")
for neighbor in neighbors:
    print(f"  Point ID: {neighbor.id()}, coords: [{neighbor.x()}, {neighbor.y()}]")
```

The wrapper also supports `Point3D`/`Kdtree3D` and `Point4D`/`Kdtree4D` for 3D and 4D spaces.

### Running the Python Demo

```bash
python demo.py
```

## Code Formatting

This repository uses clang-format for C++ code formatting. To set up the pre-commit hook:

```bash
# Install the git hook
./install-hooks.sh
```

Once installed, the pre-commit hook will automatically check that all C++ files are properly formatted before allowing a commit. If formatting issues are found, run:

```bash
clang-format -i *.cc *.h
```
