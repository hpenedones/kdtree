#!/usr/bin/env python3
"""
Example demonstrating the Python wrapper for the kdtree library.
"""

try:
    import pykdtree
except ImportError:
    print("Error: pykdtree module not found.")
    print("Please build and install the Python wrapper first:")
    print("  pip install pybind11")
    print("  python setup.py install")
    exit(1)

import random


def demo_2d():
    """Demonstrate 2D k-d tree usage."""
    print("=== 2D K-d Tree Demo ===\n")

    # Create points
    p1 = pykdtree.Point2D(1, [0.3, 0.5])
    p2 = pykdtree.Point2D(2, [-0.3, 0.5])
    p3 = pykdtree.Point2D(3, [0.9, 1.5])

    print(f"Created points:")
    print(f"  {p1}")
    print(f"  {p2}")
    print(f"  {p3}")

    # Build tree
    tree = pykdtree.Kdtree2D(p1)
    tree.insert(p2)
    tree.insert(p3)
    print("\nBuilt k-d tree with 3 points")

    # Query
    query = pykdtree.Point2D(10, [1.3, 0.5])
    radius = 1.0
    neighbors = tree.get_nearby_points(query, radius)

    print(f"\nQuery point: {query}")
    print(f"Search radius: {radius}")
    print(f"Found {len(neighbors)} neighbors:")
    for neighbor in neighbors:
        print(f"  {neighbor}")


def demo_3d():
    """Demonstrate 3D k-d tree usage."""
    print("\n=== 3D K-d Tree Demo ===\n")

    # Create points
    p1 = pykdtree.Point3D(1, [0.3, 0.5, 0.2])
    p2 = pykdtree.Point3D(2, [-0.3, 0.5, 0.1])
    p3 = pykdtree.Point3D(3, [0.9, 1.5, 0.8])

    print(f"Created 3D points:")
    print(f"  {p1}")
    print(f"  {p2}")
    print(f"  {p3}")

    # Build tree
    tree = pykdtree.Kdtree3D(p1)
    tree.insert(p2)
    tree.insert(p3)
    print("\nBuilt 3D k-d tree with 3 points")

    # Query
    query = pykdtree.Point3D(10, [1.3, 0.5, 0.2])
    radius = 1.0
    neighbors = tree.get_nearby_points(query, radius)

    print(f"\nQuery point: {query}")
    print(f"Search radius: {radius}")
    print(f"Found {len(neighbors)} neighbors:")
    for neighbor in neighbors:
        print(f"  {neighbor}")


def benchmark_2d():
    """Simple benchmark comparing k-d tree to brute force."""
    print("\n=== 2D Benchmark ===\n")

    import time

    N = 10000
    print(f"Generating {N} random 2D points...")

    # Generate random points
    points = []
    for i in range(N):
        x = random.uniform(-1000, 1000)
        y = random.uniform(-1000, 1000)
        points.append(pykdtree.Point2D(i, [x, y]))

    # Build tree
    print("Building k-d tree...")
    start = time.time()
    tree = pykdtree.Kdtree2D(points[0])
    for i in range(1, N):
        tree.insert(points[i])
    build_time = time.time() - start
    print(f"Built k-d tree in {build_time*1000:.2f} ms")

    # Run queries
    num_queries = 100
    radius = 10.0

    print(f"\nRunning {num_queries} queries with radius {radius}...")
    start = time.time()
    total_neighbors = 0
    for i in range(num_queries):
        qx = random.uniform(-1000, 1000)
        qy = random.uniform(-1000, 1000)
        query = pykdtree.Point2D(N + i, [qx, qy])
        neighbors = tree.get_nearby_points(query, radius)
        total_neighbors += len(neighbors)
    query_time = time.time() - start

    avg_neighbors = total_neighbors / num_queries
    print(f"K-d tree: {query_time*1000:.2f} ms total ({query_time*1000/num_queries:.3f} ms/query)")
    print(f"Average neighbors found: {avg_neighbors:.1f}")


if __name__ == "__main__":
    demo_2d()
    demo_3d()
    benchmark_2d()
    print("\n✓ All demos completed successfully!")
