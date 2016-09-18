#ifndef KDTREE_H
#define KDTREE_H

#include <list>

#include "point.h"

class Kdtree {

    public:

       
    Kdtree(const Point & point, int split_axis = 0);
    ~Kdtree();
    void insert(Point & point); 
    std::list<Point> get_nearby_points(const Point & point, int radius) const;
    std::list<int> get_nearby_point_ids(const Point & point, int radius) const;
    
    private:

    bool intersects_line(const Point & point, int radius) const;
    bool is_neighbor(const Point & point, int radius) const;
    bool falls_on_left_child(const Point & point) const;
    int  split_value() const;
    
    int split_axis;
    const Point & split_point;

    Kdtree * left;
    Kdtree * right;

};

#endif // KDTREE_H
