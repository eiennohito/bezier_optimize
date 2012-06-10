#ifndef geometry_h__22a6b386_9164_42e9_b9e9_50eee7c736d9
#define geometry_h__22a6b386_9164_42e9_b9e9_50eee7c736d9

#include <boost/operators.hpp>

struct Point2: boost::additive<Point2>, boost::multiplicative<Point2, float> {
  float x;
  float y;

  Point2& operator +=(const Point2& o) { x += o.x; y += o.y; return *this; }
  Point2& operator -=(const Point2& o) { x -= o.x; y -= o.y; return *this; }
  Point2& operator *=(const float f) { x *= f; y *= f; return *this; }
  Point2& operator /=(const float f) { x /= f; y /= f; return *this; }

  Point2(): x(0), y(0) {}
  template<typename T>
  Point2(T x_, T y_): x(static_cast<float>(x_)), y(static_cast<float>(y_)) {}
  
  Point2(float f[2]): x(f[0]), y(f[1]) {}

  Point2(const Point2& o): x(o.x), y(o.y) {}
  Point2& operator=(const Point2& o) { x = o.x; y = o.y; return *this; }
};

typedef Point2 Vector2;

float dot_prod(const Vector2& v1, const Vector2& v2);
float cross_prod(const Vector2& v1, const Vector2& v2);

float sqdist(const Point2& p1, const Point2& p2);

template<typename Str>
Str& operator<<(Str& str, const Point2& pt) {
  str << static_cast<int>(pt.x) << "," static_cast<int>(pt.y);
}

#endif // geometry_h__22a6b386_9164_42e9_b9e9_50eee7c736d9
