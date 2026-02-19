
#include "kdtree.h"


Kdtree::Kdtree(const Point& point, int axis):
    left(nullptr), right(nullptr), split_point(point), split_axis(axis)
{

}

Kdtree::~Kdtree()
{
    if (left)  delete left;
    if (right) delete right;
}

float Kdtree::split_value() const
{
    if (split_axis == 0) return split_point.x();

    return split_point.y();
}

// inserts a new point into the kdtree data structure
void Kdtree::insert(const Point & new_point)
{
    // new split axis for the subtree
    int new_split_axis = (split_axis + 1) % 2;

    // deciding on left or right subtree
    if ( falls_on_left_child(new_point))
    {
        if (!left)
        {
            left = new Kdtree(new_point, new_split_axis);
            return;
        }
        left->insert(new_point);
    }
    else
    {
        if (!right)
        {
            right = new Kdtree(new_point, new_split_axis);
            return;
        }
        right->insert(new_point);
    }
}

// check whether a given circle (defined by a point and a radius) intersects
// the line defined by this kdtree node (defined by its point and split_axis)
bool Kdtree::intersects_line(const Point & point, float radius) const
{
    float value = (split_axis == 0) ? point.x() : point.y();
    float split = split_value();

    return (value - radius <= split && split <= value + radius);
}

bool Kdtree::is_neighbor(const Point & point, float radius) const
{
    float xdif = split_point.x() - point.x();
    float ydif = split_point.y() - point.y();

    // using Euclidean distance
    return (xdif * xdif + ydif * ydif <= radius * radius);
}




bool Kdtree::falls_on_left_child(const Point & point) const
{
    float value = (split_axis == 0) ? point.x() : point.y();

    return value < split_value();
}


std::list<Point> Kdtree::get_nearby_points(const Point & point, float radius) const
{
    std::list<Point> result;

    if (is_neighbor(point, radius))
        result.push_back(split_point);

    // If there is a left child, we have to visit it recursively if the test "circle" 
    // falls at least partially on the left side of the decision line.
    // This is an opportunity for pruning the search!
    if (left && (intersects_line(point, radius) || falls_on_left_child(point) ) )
    {
        std::list<Point> left_result = left->get_nearby_points(point, radius);
        result.insert(result.end(), left_result.begin(), left_result.end());
    }

    // Same thing for the right side.
    if (right && (intersects_line(point, radius) || !falls_on_left_child(point)))
    {
        std::list<Point> right_result = right->get_nearby_points(point, radius);
        result.insert(result.end(), right_result.begin(), right_result.end());
    }

    return result;
}

