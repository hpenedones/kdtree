#ifndef POINT_H
#define POINT_H

class Point
{

public:

   Point(int id, float x, float y)    :
      m_id(id), m_x(x), m_y(y) {

   }

   int id() const {
      return m_id;
   };

   float x() const {
      return m_x;
   };

   float y() const {
      return m_y;
   };

private:

   int m_id;
   float m_x;
   float m_y;
};

#endif // POINT_H
