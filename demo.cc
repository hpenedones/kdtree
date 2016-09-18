
#include <list>
#include <iostream>
#include <vector>

#include "point.h"
#include "kdtree.h"

using namespace std;

int main(int argc, char* argv[])
{

  std::vector<Point> points {
    Point(1, 0.3, 0.5),
    Point(2, -0.3, 0.5),
    Point(3, 0.9, 1.5),
    Point(4, 1.7, 1.5),
    Point(5, 3.3, 0.95),
    Point(6, 0.03, -0.5),
  };


  Kdtree * kdtree = new Kdtree(points[0]);

  for (int i = 1; i < points.size(); i++ ) {
    kdtree->insert(points[i]);
  }

  Point query_point = Point(10, 1.3, 0.5);
  float radius = 0.9;

  auto neighbors = kdtree->get_nearby_points(query_point, radius);

  for (auto & p : neighbors)
  {
      std::cout << p.id() << " " << p.x() << " " << p.y() << std::endl; 
  }

  delete kdtree;

  return 0;
}
