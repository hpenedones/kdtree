#ifndef KDTREE_H
#define KDTREE_H

#include <list>

#include "point.h"

class Kdtree {

public:

    Kdtree(const Point & point, int split_axis = 0);
    ~Kdtree();
    void insert(const Point & point);
    std::list<Point> get_nearby_points(const Point & point, int radius) const;

private:

    bool intersects_line(const Point & point, int radius) const;
    bool is_neighbor(const Point & point, int radius) const;
    bool falls_on_left_child(const Point & point) const;
    int  split_value() const;

    Kdtree * left;
    Kdtree * right;

    const Point & split_point;
    int split_axis;
};

#endif // KDTREE_H
