// Unit tests for Kdtree.
// Each test function returns true on success and false (with a message) on failure.
// The main() exits with a non-zero status if any test fails.

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <vector>

#include "kdtree.h"
#include "point.h"

// ---- minimal test helpers ----

static int g_failures = 0;

#define CHECK(expr)                                                       \
    do {                                                                  \
        if (!(expr)) {                                                    \
            std::cerr << "FAIL [" << __func__ << "] " << #expr << "\n"; \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

#define CHECK_EQ(a, b)                                                    \
    do {                                                                  \
        if ((a) != (b)) {                                                 \
            std::cerr << "FAIL [" << __func__ << "] " << #a << " == "   \
                      << #b << "  (" << (a) << " != " << (b) << ")\n";  \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

// ---- brute-force helper ----

static std::vector<int> brute_ids(const std::vector<Point> &pts,
                                   const Point &q, float r) {
    std::vector<int> ids;
    for (const auto &p : pts) {
        float dx = p.x() - q.x();
        float dy = p.y() - q.y();
        if (dx * dx + dy * dy <= r * r)
            ids.push_back(p.id());
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

static std::vector<int> kdtree_ids(const std::vector<Point> &pts,
                                    const Point &q, float r) {
    Kdtree tree(pts[0]);
    for (size_t i = 1; i < pts.size(); ++i)
        tree.insert(pts[i]);
    std::list<Point> result = tree.get_nearby_points(q, r);
    std::vector<int> ids;
    for (const auto &p : result)
        ids.push_back(p.id());
    std::sort(ids.begin(), ids.end());
    return ids;
}

// ---- individual tests ----

static void test_single_point_within_radius() {
    // A tree with one point; query from the same location should find it.
    Kdtree tree(Point(1, 0.0f, 0.0f));
    auto result = tree.get_nearby_points(Point(99, 0.0f, 0.0f), 0.1f);
    CHECK_EQ((int)result.size(), 1);
    CHECK_EQ(result.front().id(), 1);
}

static void test_single_point_outside_radius() {
    Kdtree tree(Point(1, 10.0f, 10.0f));
    auto result = tree.get_nearby_points(Point(99, 0.0f, 0.0f), 1.0f);
    CHECK_EQ((int)result.size(), 0);
}

static void test_point_exactly_on_boundary_included() {
    // is_neighbor uses <=, so a point at exactly distance=radius must be included.
    Kdtree tree(Point(1, 1.0f, 0.0f));
    Point query(99, 0.0f, 0.0f);
    float radius = 1.0f;  // distance to (1,0) is exactly 1.0
    auto result = tree.get_nearby_points(query, radius);
    CHECK_EQ((int)result.size(), 1);
}

static void test_large_radius_returns_all() {
    std::vector<Point> pts = {
        Point(1, 0.0f, 0.0f), Point(2, 5.0f, 5.0f),
        Point(3, -5.0f, 3.0f), Point(4, 2.0f, -4.0f),
    };
    auto kd = kdtree_ids(pts, Point(99, 0.0f, 0.0f), 100.0f);
    CHECK_EQ((int)kd.size(), 4);
}

static void test_zero_radius_returns_only_coincident() {
    std::vector<Point> pts = {
        Point(1, 0.0f, 0.0f), Point(2, 1.0f, 0.0f), Point(3, 0.0f, 1.0f),
    };
    // Only pt 1 is at the query location.
    auto kd = kdtree_ids(pts, Point(99, 0.0f, 0.0f), 0.0f);
    CHECK_EQ((int)kd.size(), 1);
    CHECK_EQ(kd[0], 1);
}

static void test_matches_brute_force_small() {
    std::vector<Point> pts = {
        Point(1, 0.3f, 0.5f),  Point(2, -0.3f, 0.5f),
        Point(3, 0.9f, 1.5f),  Point(4, 1.7f, 1.5f),
        Point(5, 3.3f, 0.95f), Point(6, 0.03f, -0.5f),
    };
    Point query(10, 1.3f, 0.5f);
    float radius = 1.1f;
    auto bf = brute_ids(pts, query, radius);
    auto kd = kdtree_ids(pts, query, radius);
    CHECK_EQ((int)kd.size(), (int)bf.size());
    CHECK(kd == bf);
}

static void test_matches_brute_force_random_large() {
    const int N = 5000;
    std::mt19937 rng(123);
    std::uniform_real_distribution<float> coord(-100.0f, 100.0f);

    std::vector<Point> pts;
    pts.reserve(N);
    for (int i = 0; i < N; ++i)
        pts.emplace_back(i, coord(rng), coord(rng));

    // Run several random queries.
    std::uniform_real_distribution<float> qd(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rd(1.0f, 30.0f);
    for (int q = 0; q < 20; ++q) {
        Point query(N + q, qd(rng), qd(rng));
        float radius = rd(rng);
        auto bf = brute_ids(pts, query, radius);
        auto kd = kdtree_ids(pts, query, radius);
        if (kd != bf) {
            std::cerr << "FAIL [test_matches_brute_force_random_large] "
                         "query #" << q << " kd=" << kd.size()
                      << " bf=" << bf.size() << "\n";
            ++g_failures;
        }
    }
}

// ---- main ----

int main() {
    test_single_point_within_radius();
    test_single_point_outside_radius();
    test_point_exactly_on_boundary_included();
    test_large_radius_returns_all();
    test_zero_radius_returns_only_coincident();
    test_matches_brute_force_small();
    test_matches_brute_force_random_large();

    if (g_failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << g_failures << " test(s) failed.\n";
    return 1;
}
