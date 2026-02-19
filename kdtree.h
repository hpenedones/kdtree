#ifndef KDTREE_H
#define KDTREE_H

#include <list>

#include "point.h"

template<int N>
class Kdtree {

public:

    Kdtree(const Point<N> & point, int split_axis = 0);
    ~Kdtree();
    void insert(const Point<N> & point);
    std::list<Point<N>> get_nearby_points(const Point<N> & point, float radius) const;

private:

    bool intersects_line(const Point<N> & point, float radius) const;
    bool is_neighbor(const Point<N> & point, float radius) const;
    bool falls_on_left_child(const Point<N> & point) const;
    float split_value() const;

    Kdtree<N> * left;
    Kdtree<N> * right;

    const Point<N> split_point;
    int split_axis;
};

// Template implementation must be in header

template<int N>
Kdtree<N>::Kdtree(const Point<N>& point, int axis):
    left(nullptr), right(nullptr), split_point(point), split_axis(axis)
{

}

template<int N>
Kdtree<N>::~Kdtree()
{
    if (left)  delete left;
    if (right) delete right;
}

template<int N>
float Kdtree<N>::split_value() const
{
    return split_point[split_axis];
}

template<int N>
void Kdtree<N>::insert(const Point<N> & new_point)
{
    // new split axis for the subtree - cycle through all N dimensions
    int new_split_axis = (split_axis + 1) % N;

    // deciding on left or right subtree
    if ( falls_on_left_child(new_point))
    {
        if (!left)
        {
            left = new Kdtree<N>(new_point, new_split_axis);
            return;
        }
        left->insert(new_point);
    }
    else
    {
        if (!right)
        {
            right = new Kdtree<N>(new_point, new_split_axis);
            return;
        }
        right->insert(new_point);
    }
}

template<int N>
bool Kdtree<N>::intersects_line(const Point<N> & point, float radius) const
{
    float value = point[split_axis];
    float split = split_value();

    return (value - radius <= split && split <= value + radius);
}

template<int N>
bool Kdtree<N>::is_neighbor(const Point<N> & point, float radius) const
{
    // N-dimensional Euclidean distance
    float squared_distance = 0.0f;
    for (int i = 0; i < N; ++i)
    {
        float diff = split_point[i] - point[i];
        squared_distance += diff * diff;
    }

    return (squared_distance <= radius * radius);
}

template<int N>
bool Kdtree<N>::falls_on_left_child(const Point<N> & point) const
{
    return point[split_axis] < split_value();
}

template<int N>
std::list<Point<N>> Kdtree<N>::get_nearby_points(const Point<N> & point, float radius) const
{
    std::list<Point<N>> result;

    if (is_neighbor(point, radius))
        result.push_back(split_point);

    // If there is a left child, we have to visit it recursively if the test sphere
    // falls at least partially on the left side of the decision hyperplane.
    // This is an opportunity for pruning the search!
    if (left && (intersects_line(point, radius) || falls_on_left_child(point) ) )
    {
        std::list<Point<N>> left_result = left->get_nearby_points(point, radius);
        result.insert(result.end(), left_result.begin(), left_result.end());
    }

    // Same thing for the right side.
    if (right && (intersects_line(point, radius) || !falls_on_left_child(point)))
    {
        std::list<Point<N>> right_result = right->get_nearby_points(point, radius);
        result.insert(result.end(), right_result.begin(), right_result.end());
    }

    return result;
}

#endif // KDTREE_H
