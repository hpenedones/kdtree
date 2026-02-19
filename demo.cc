
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <list>
#include <random>
#include <vector>

#include "kdtree.h"
#include "point.h"

// Brute-force reference implementation for comparison.
static std::vector<Point> brute_force_nearby(const std::vector<Point> &points,
                                              const Point &query, float radius) {
    std::vector<Point> result;
    for (const auto &p : points) {
        float dx = p.x() - query.x();
        float dy = p.y() - query.y();
        if (dx * dx + dy * dy <= radius * radius)
            result.push_back(p);
    }
    return result;
}

int main() {
    const int N = 100000;
    const float COORD_RANGE = 1000.0f;
    const float QUERY_RADIUS = 10.0f;

    // Generate N random points uniformly in [-COORD_RANGE, COORD_RANGE]^2.
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-COORD_RANGE, COORD_RANGE);

    std::vector<Point> points;
    points.reserve(N);
    for (int i = 0; i < N; ++i)
        points.emplace_back(i, dist(rng), dist(rng));

    // ---- Build the k-d tree ----
    auto build_start = std::chrono::high_resolution_clock::now();
    Kdtree *kdtree = new Kdtree(points[0]);
    for (int i = 1; i < N; ++i)
        kdtree->insert(points[i]);
    auto build_end = std::chrono::high_resolution_clock::now();
    double build_ms =
        std::chrono::duration<double, std::milli>(build_end - build_start).count();

    std::cout << "Built k-d tree with " << N << " points in "
              << build_ms << " ms\n";

    // Generate query points.
    const int NUM_QUERIES = 1000;
    std::vector<Point> queries;
    queries.reserve(NUM_QUERIES);
    for (int i = 0; i < NUM_QUERIES; ++i)
        queries.emplace_back(N + i, dist(rng), dist(rng));

    // ---- Queries: k-d tree ----
    size_t kd_total = 0;
    auto kd_start = std::chrono::high_resolution_clock::now();
    for (const auto &q : queries)
        kd_total += kdtree->get_nearby_points(q, QUERY_RADIUS).size();
    auto kd_end = std::chrono::high_resolution_clock::now();
    double kd_ms =
        std::chrono::duration<double, std::milli>(kd_end - kd_start).count();

    // ---- Queries: brute force ----
    size_t bf_total = 0;
    auto bf_start = std::chrono::high_resolution_clock::now();
    for (const auto &q : queries)
        bf_total += brute_force_nearby(points, q, QUERY_RADIUS).size();
    auto bf_end = std::chrono::high_resolution_clock::now();
    double bf_ms =
        std::chrono::duration<double, std::milli>(bf_end - bf_start).count();

    std::cout << NUM_QUERIES << " range queries (radius=" << QUERY_RADIUS << "):\n";
    std::cout << "  K-d tree   : " << kd_ms << " ms total  ("
              << kd_ms / NUM_QUERIES << " ms/query, "
              << kd_total / NUM_QUERIES << " neighbors avg)\n";
    std::cout << "  Brute force: " << bf_ms << " ms total  ("
              << bf_ms / NUM_QUERIES << " ms/query, "
              << bf_total / NUM_QUERIES << " neighbors avg)\n";

    if (kd_total == bf_total)
        std::cout << "Results match ✓\n";
    else
        std::cout << "Results MISMATCH ✗ (kdtree=" << kd_total
                  << " brute=" << bf_total << ")\n";

    if (bf_ms > 0.0)
        std::cout << "Speedup: " << (bf_ms / kd_ms) << "x\n";

    delete kdtree;
    return 0;
}
