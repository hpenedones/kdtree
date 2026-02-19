#ifndef KDTREE_H
#define KDTREE_H

#include <list>

#include "point.h"

class Kdtree {

public:

    Kdtree(const Point & point, int split_axis = 0);
    ~Kdtree();
    void insert(const Point & point);
    std::list<Point> get_nearby_points(const Point & point, float radius) const;

private:

    bool intersects_line(const Point & point, float radius) const;
    bool is_neighbor(const Point & point, float radius) const;
    bool falls_on_left_child(const Point & point) const;
    float split_value() const;

    Kdtree * left;
    Kdtree * right;

    const Point split_point;
    int split_axis;
};

#endif // KDTREE_H
