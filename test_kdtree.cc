// Unit tests for Kdtree.
// Each test function increments g_failures on any assertion failure.
// The main() exits with a non-zero status if any test fails.

#include <algorithm>
#include <iostream>
#include <list>
#include <random>
#include <vector>

#include "kdtree.h"
#include "point.h"

// Type aliases for convenience
using Point2D = Point<2>;
using Point3D = Point<3>;
using Kdtree2D = Kdtree<2>;
using Kdtree3D = Kdtree<3>;

// ---- minimal test helpers ----

static int g_failures = 0;

#define CHECK(expr)                                                       \
    do {                                                                  \
        auto _val = (expr);                                               \
        if (!_val) {                                                      \
            std::cerr << "FAIL [" << __func__ << "] " << #expr << "\n"; \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

#define CHECK_EQ(a, b)                                                    \
    do {                                                                  \
        auto _a = (a);                                                    \
        auto _b = (b);                                                    \
        if (_a != _b) {                                                   \
            std::cerr << "FAIL [" << __func__ << "] " << #a << " == "   \
                      << #b << "  (" << _a << " != " << _b << ")\n";    \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

// ---- brute-force helper for 2D ----

static std::vector<int> brute_ids_2d(const std::vector<Point2D> &pts,
                                   const Point2D &q, float r) {
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

static std::vector<int> kdtree_ids_2d(const std::vector<Point2D> &pts,
                                    const Point2D &q, float r) {
    Kdtree2D tree(pts[0]);
    for (size_t i = 1; i < pts.size(); ++i)
        tree.insert(pts[i]);
    std::list<Point2D> result = tree.get_nearby_points(q, r);
    std::vector<int> ids;
    for (const auto &p : result)
        ids.push_back(p.id());
    std::sort(ids.begin(), ids.end());
    return ids;
}

// ---- brute-force helper for 3D ----

static std::vector<int> brute_ids_3d(const std::vector<Point3D> &pts,
                                   const Point3D &q, float r) {
    std::vector<int> ids;
    for (const auto &p : pts) {
        float dx = p.x() - q.x();
        float dy = p.y() - q.y();
        float dz = p.z() - q.z();
        if (dx * dx + dy * dy + dz * dz <= r * r)
            ids.push_back(p.id());
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

static std::vector<int> kdtree_ids_3d(const std::vector<Point3D> &pts,
                                    const Point3D &q, float r) {
    Kdtree3D tree(pts[0]);
    for (size_t i = 1; i < pts.size(); ++i)
        tree.insert(pts[i]);
    std::list<Point3D> result = tree.get_nearby_points(q, r);
    std::vector<int> ids;
    for (const auto &p : result)
        ids.push_back(p.id());
    std::sort(ids.begin(), ids.end());
    return ids;
}

// ---- 2D tests ----

static void test_single_point_within_radius() {
    // A tree with one point; query from the same location should find it.
    Kdtree2D tree(Point2D(1, {{0.0f, 0.0f}}));
    auto result = tree.get_nearby_points(Point2D(99, {{0.0f, 0.0f}}), 0.1f);
    CHECK_EQ((int)result.size(), 1);
    CHECK_EQ(result.front().id(), 1);
}

static void test_single_point_outside_radius() {
    Kdtree2D tree(Point2D(1, {{10.0f, 10.0f}}));
    auto result = tree.get_nearby_points(Point2D(99, {{0.0f, 0.0f}}), 1.0f);
    CHECK_EQ((int)result.size(), 0);
}

static void test_point_exactly_on_boundary_included() {
    // is_neighbor uses <=, so a point at exactly distance=radius must be included.
    Kdtree2D tree(Point2D(1, {{1.0f, 0.0f}}));
    Point2D query(99, {{0.0f, 0.0f}});
    float radius = 1.0f;  // distance to (1,0) is exactly 1.0
    auto result = tree.get_nearby_points(query, radius);
    CHECK_EQ((int)result.size(), 1);
}

static void test_large_radius_returns_all() {
    std::vector<Point2D> pts = {
        Point2D(1, {{0.0f, 0.0f}}), Point2D(2, {{5.0f, 5.0f}}),
        Point2D(3, {{-5.0f, 3.0f}}), Point2D(4, {{2.0f, -4.0f}}),
    };
    auto kd = kdtree_ids_2d(pts, Point2D(99, {{0.0f, 0.0f}}), 100.0f);
    CHECK_EQ((int)kd.size(), 4);
}

static void test_zero_radius_returns_only_coincident() {
    std::vector<Point2D> pts = {
        Point2D(1, {{0.0f, 0.0f}}), Point2D(2, {{1.0f, 0.0f}}), Point2D(3, {{0.0f, 1.0f}}),
    };
    // Only pt 1 is at the query location.
    auto kd = kdtree_ids_2d(pts, Point2D(99, {{0.0f, 0.0f}}), 0.0f);
    CHECK_EQ((int)kd.size(), 1);
    CHECK_EQ(kd[0], 1);
}

static void test_matches_brute_force_small() {
    std::vector<Point2D> pts = {
        Point2D(1, {{0.3f, 0.5f}}),  Point2D(2, {{-0.3f, 0.5f}}),
        Point2D(3, {{0.9f, 1.5f}}),  Point2D(4, {{1.7f, 1.5f}}),
        Point2D(5, {{3.3f, 0.95f}}), Point2D(6, {{0.03f, -0.5f}}),
    };
    Point2D query(10, {{1.3f, 0.5f}});
    float radius = 1.1f;
    auto bf = brute_ids_2d(pts, query, radius);
    auto kd = kdtree_ids_2d(pts, query, radius);
    CHECK_EQ((int)kd.size(), (int)bf.size());
    CHECK(kd == bf);
}

static void test_matches_brute_force_random_large() {
    const int N = 5000;
    std::mt19937 rng(123);
    std::uniform_real_distribution<float> coord(-100.0f, 100.0f);

    std::vector<Point2D> pts;
    pts.reserve(N);
    for (int i = 0; i < N; ++i)
        pts.emplace_back(i, std::array<float, 2>{{coord(rng), coord(rng)}});

    // Run several random queries.
    std::uniform_real_distribution<float> qd(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rd(1.0f, 30.0f);
    for (int q = 0; q < 20; ++q) {
        Point2D query(N + q, {{qd(rng), qd(rng)}});
        float radius = rd(rng);
        auto bf = brute_ids_2d(pts, query, radius);
        auto kd = kdtree_ids_2d(pts, query, radius);
        if (kd != bf) {
            std::cerr << "FAIL [test_matches_brute_force_random_large] "
                         "query #" << q << " kd=" << kd.size()
                      << " bf=" << bf.size() << "\n";
            ++g_failures;
        }
    }
}

// ---- 3D tests ----

static void test_3d_single_point_within_radius() {
    Kdtree3D tree(Point3D(1, {{0.0f, 0.0f, 0.0f}}));
    auto result = tree.get_nearby_points(Point3D(99, {{0.0f, 0.0f, 0.0f}}), 0.1f);
    CHECK_EQ((int)result.size(), 1);
    CHECK_EQ(result.front().id(), 1);
}

static void test_3d_single_point_outside_radius() {
    Kdtree3D tree(Point3D(1, {{10.0f, 10.0f, 10.0f}}));
    auto result = tree.get_nearby_points(Point3D(99, {{0.0f, 0.0f, 0.0f}}), 1.0f);
    CHECK_EQ((int)result.size(), 0);
}

static void test_3d_point_on_boundary() {
    // Point at (1,0,0) is exactly distance 1.0 from origin
    Kdtree3D tree(Point3D(1, {{1.0f, 0.0f, 0.0f}}));
    Point3D query(99, {{0.0f, 0.0f, 0.0f}});
    float radius = 1.0f;
    auto result = tree.get_nearby_points(query, radius);
    CHECK_EQ((int)result.size(), 1);
}

static void test_3d_matches_brute_force_small() {
    std::vector<Point3D> pts = {
        Point3D(1, {{0.0f, 0.0f, 0.0f}}),
        Point3D(2, {{1.0f, 0.0f, 0.0f}}),
        Point3D(3, {{0.0f, 1.0f, 0.0f}}),
        Point3D(4, {{0.0f, 0.0f, 1.0f}}),
        Point3D(5, {{5.0f, 5.0f, 5.0f}}),
    };
    Point3D query(10, {{0.5f, 0.5f, 0.5f}});
    float radius = 1.5f;
    auto bf = brute_ids_3d(pts, query, radius);
    auto kd = kdtree_ids_3d(pts, query, radius);
    CHECK_EQ((int)kd.size(), (int)bf.size());
    CHECK(kd == bf);
}

static void test_3d_matches_brute_force_random() {
    const int N = 1000;
    std::mt19937 rng(456);
    std::uniform_real_distribution<float> coord(-50.0f, 50.0f);

    std::vector<Point3D> pts;
    pts.reserve(N);
    for (int i = 0; i < N; ++i)
        pts.emplace_back(i, std::array<float, 3>{{coord(rng), coord(rng), coord(rng)}});

    // Run several random queries.
    std::uniform_real_distribution<float> qd(-50.0f, 50.0f);
    std::uniform_real_distribution<float> rd(1.0f, 20.0f);
    for (int q = 0; q < 10; ++q) {
        Point3D query(N + q, {{qd(rng), qd(rng), qd(rng)}});
        float radius = rd(rng);
        auto bf = brute_ids_3d(pts, query, radius);
        auto kd = kdtree_ids_3d(pts, query, radius);
        if (kd != bf) {
            std::cerr << "FAIL [test_3d_matches_brute_force_random] "
                         "query #" << q << " kd=" << kd.size()
                      << " bf=" << bf.size() << "\n";
            ++g_failures;
        }
    }
}

// ---- main ----

int main() {
    // 2D tests
    test_single_point_within_radius();
    test_single_point_outside_radius();
    test_point_exactly_on_boundary_included();
    test_large_radius_returns_all();
    test_zero_radius_returns_only_coincident();
    test_matches_brute_force_small();
    test_matches_brute_force_random_large();

    // 3D tests
    test_3d_single_point_within_radius();
    test_3d_single_point_outside_radius();
    test_3d_point_on_boundary();
    test_3d_matches_brute_force_small();
    test_3d_matches_brute_force_random();

    if (g_failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << g_failures << " test(s) failed.\n";
    return 1;
}
