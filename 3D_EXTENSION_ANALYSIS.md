# Extending K-d Tree to 3D Points

## Executive Summary

Extending this k-d tree implementation from 2D to 3D is **relatively straightforward** and does not require complicated changes to the core algorithms. The main changes involve:

1. Adding a `z` coordinate to the Point class
2. Updating the split axis cycling from 2 to 3 dimensions
3. Extending distance calculations to include the z-component
4. Updating intersection tests to handle 3D hyperplanes

**Algorithmic Complexity**: The core k-d tree algorithms remain unchanged in their logic and complexity:
- **Construction**: Still O(n log n) with balanced insertion
- **Range query**: Still O(n^(2/3) + k) on average for 3D (vs O(n^(1/2) + k) for 2D)
- **Space**: Still O(n)

## Current 2D Implementation Overview

The implementation consists of:
- **Point class**: Stores (x, y) coordinates and an ID
- **Kdtree class**: Binary tree where each node splits space along alternating axes (x→y→x→y...)
- **Core operations**:
  - `insert()`: Adds points by recursively choosing left/right subtree based on split axis
  - `get_nearby_points()`: Range query using sphere (circle in 2D) with radius-based pruning
  - `intersects_line()`: Tests if query sphere crosses the splitting hyperplane (line in 2D)
  - `is_neighbor()`: Euclidean distance check
  - `falls_on_left_child()`: Determines which side of split a point falls on

## Required Changes for 3D

### 1. Point Class (point.h)

**Current (2D)**:
```cpp
class Point {
public:
   Point(int id, float x, float y) : m_id(id), m_x(x), m_y(y) {}

   float x() const { return m_x; }
   float y() const { return m_y; }

private:
   int m_id;
   float m_x;
   float m_y;
};
```

**Proposed (3D)**:
```cpp
class Point {
public:
   Point(int id, float x, float y, float z) :
      m_id(id), m_x(x), m_y(y), m_z(z) {}

   float x() const { return m_x; }
   float y() const { return m_y; }
   float z() const { return m_z; }  // NEW

private:
   int m_id;
   float m_x;
   float m_y;
   float m_z;  // NEW
};
```

**Change complexity**: ⭐ Simple - just add one member variable and accessor.

---

### 2. Kdtree Class - Split Axis Cycling (kdtree.cc)

**Current (2D)** - cycles between 2 axes:
```cpp
void Kdtree::insert(const Point & new_point) {
    // Cycle through 2 dimensions: 0 -> 1 -> 0 -> 1 ...
    int new_split_axis = (split_axis + 1) % 2;
    // ... rest of insertion logic
}
```

**Proposed (3D)** - cycles between 3 axes:
```cpp
void Kdtree::insert(const Point & new_point) {
    // Cycle through 3 dimensions: 0 -> 1 -> 2 -> 0 -> 1 -> 2 ...
    int new_split_axis = (split_axis + 1) % 3;  // CHANGE: 2 -> 3
    // ... rest of insertion logic unchanged
}
```

**Change complexity**: ⭐ Trivial - change one constant.

---

### 3. Split Value Function (kdtree.cc)

**Current (2D)**:
```cpp
float Kdtree::split_value() const {
    if (split_axis == 0) return split_point.x();
    return split_point.y();
}
```

**Proposed (3D)**:
```cpp
float Kdtree::split_value() const {
    if (split_axis == 0) return split_point.x();
    if (split_axis == 1) return split_point.y();
    return split_point.z();  // NEW
}
```

**Change complexity**: ⭐ Simple - add one more case.

---

### 4. Distance Calculation (kdtree.cc)

**Current (2D)** - Euclidean distance in 2D:
```cpp
bool Kdtree::is_neighbor(const Point & point, float radius) const {
    float xdif = split_point.x() - point.x();
    float ydif = split_point.y() - point.y();

    // 2D Euclidean distance
    return (xdif * xdif + ydif * ydif <= radius * radius);
}
```

**Proposed (3D)** - Euclidean distance in 3D:
```cpp
bool Kdtree::is_neighbor(const Point & point, float radius) const {
    float xdif = split_point.x() - point.x();
    float ydif = split_point.y() - point.y();
    float zdif = split_point.z() - point.z();  // NEW

    // 3D Euclidean distance
    return (xdif * xdif + ydif * ydif + zdif * zdif <= radius * radius);
}
```

**Change complexity**: ⭐ Simple - add z-component to distance calculation.

---

### 5. Point Classification (kdtree.cc)

**Current (2D)**:
```cpp
bool Kdtree::falls_on_left_child(const Point & point) const {
    float value = (split_axis == 0) ? point.x() : point.y();
    return value < split_value();
}
```

**Proposed (3D)**:
```cpp
bool Kdtree::falls_on_left_child(const Point & point) const {
    float value;
    if (split_axis == 0) value = point.x();
    else if (split_axis == 1) value = point.y();
    else value = point.z();  // NEW

    return value < split_value();
}
```

**Change complexity**: ⭐ Simple - handle third axis case.

---

### 6. Hyperplane Intersection (kdtree.cc)

**Current (2D)** - tests if circle intersects a line:
```cpp
bool Kdtree::intersects_line(const Point & point, float radius) const {
    float value = (split_axis == 0) ? point.x() : point.y();
    float split = split_value();

    return (value - radius <= split && split <= value + radius);
}
```

**Proposed (3D)** - tests if sphere intersects a plane:
```cpp
bool Kdtree::intersects_line(const Point & point, float radius) const {
    float value;
    if (split_axis == 0) value = point.x();
    else if (split_axis == 1) value = point.y();
    else value = point.z();  // NEW

    float split = split_value();

    // Same logic: sphere intersects plane if distance < radius
    return (value - radius <= split && split <= value + radius);
}
```

**Change complexity**: ⭐ Simple - same intersection logic, just handle z-axis.

**Why it works**: In 3D, each k-d tree node splits 3D space with a **plane** perpendicular to one axis (e.g., x=5.0 is a yz-plane). The intersection test is identical: we check if the query sphere crosses this plane by comparing the perpendicular distance to the radius.

---

## Algorithm Analysis

### Why the Changes Are Not Complicated

1. **Tree structure unchanged**: Still a binary tree with left/right children
2. **Splitting logic unchanged**: Still alternates axes (just 3 instead of 2)
3. **Distance metric unchanged**: Still Euclidean distance (just add z² term)
4. **Search pruning unchanged**: Still prunes subtrees when impossible to contain neighbors
5. **No new data structures needed**: Same std::list for results

### Performance Characteristics

**2D vs 3D comparison**:

| Aspect | 2D | 3D | Note |
|--------|----|----|------|
| Construction | O(n log n) | O(n log n) | Same |
| Space | O(n) | O(n) | Same |
| Range query (avg) | O(√n + k) | O(n^(2/3) + k) | Worse in 3D |
| Range query (worst) | O(n) | O(n) | Same |

The query performance degrades slightly in 3D due to the "curse of dimensionality" - higher-dimensional spaces have more volume, so pruning is less effective. However, k-d trees are still very efficient for 3D (they become less useful in 5+ dimensions).

### Testing Considerations

All existing tests would work identically in 3D with minor modifications:

```cpp
// 2D test:
Kdtree tree(Point(1, 0.0f, 0.0f));

// 3D version:
Kdtree tree(Point(1, 0.0f, 0.0f, 0.0f));  // Just add z=0
```

The brute force comparison in tests would also need z-component:
```cpp
// Brute force distance check in 3D:
float dx = p.x() - query.x();
float dy = p.y() - query.y();
float dz = p.z() - query.z();  // NEW
if (dx*dx + dy*dy + dz*dz <= r*r)
    result.push_back(p);
```

---

## Summary of Changes

**Files to modify**:
1. `point.h` - Add z coordinate (3 lines)
2. `kdtree.cc` - Update 5 functions (≈15 lines)
3. `demo.cc` - Update random generation and brute force (≈5 lines)
4. `test_kdtree.cc` - Update test points and brute force (≈5 lines)
5. `README.md` - Update documentation

**Total estimated changes**: ~30-40 lines of code

**Complexity rating**: ⭐⭐ (2/5 stars)
- No algorithmic changes needed
- No new data structures
- Mostly mechanical additions of z-handling
- Core tree logic completely unchanged

**Biggest challenge**: Ensuring all test cases are updated consistently and that the z-dimension is handled in all the right places.

---

## Alternative: Generic N-Dimensional Implementation

For a more flexible solution, you could make the implementation generic:

```cpp
template<int N>
class Point {
    int m_id;
    std::array<float, N> m_coords;
public:
    Point(int id, std::array<float, N> coords) : m_id(id), m_coords(coords) {}
    float coord(int axis) const { return m_coords[axis]; }
};

template<int N>
class Kdtree {
    // split_axis cycles through [0, N)
    int new_split_axis = (split_axis + 1) % N;
    // ...
};
```

This would support 2D, 3D, or any N-dimensional points with the same code, but adds template complexity. For just 2D→3D, the explicit approach is simpler and more readable.

---

## Conclusion

Extending to 3D is **not complicated** - it's mostly a mechanical process of:
1. Adding a z coordinate everywhere x and y appear
2. Changing `% 2` to `% 3` for axis cycling
3. Adding z-handling to the three if-statements that check split_axis

The core k-d tree algorithms (binary tree structure, recursive insertion, pruned range search) require **no conceptual changes**. The geometric intuition is identical: we're just working with planes instead of lines, and spheres instead of circles.
