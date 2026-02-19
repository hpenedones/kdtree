#ifndef POINT_H
#define POINT_H

#include <array>

template<int N>
class Point
{

public:

   Point(int id, std::array<float, N> coords) :
      m_id(id), m_coords(coords) {

   }

   int id() const {
      return m_id;
   };

   float operator[](int axis) const {
      return m_coords[axis];
   };

   // Convenience accessors for common cases
   float x() const {
      static_assert(N >= 1, "x() requires at least 1 dimension");
      return m_coords[0];
   };

   float y() const {
      static_assert(N >= 2, "y() requires at least 2 dimensions");
      return m_coords[1];
   };

   float z() const {
      static_assert(N >= 3, "z() requires at least 3 dimensions");
      return m_coords[2];
   };

   static constexpr int dimensions() {
      return N;
   }

private:

   int m_id;
   std::array<float, N> m_coords;
};

#endif // POINT_H
