#ifndef Point2_h__69f46184_9958_46ee_8498_ca1ba6579272
#define Point2_h__69f46184_9958_46ee_8498_ca1ba6579272
#pragma once
#include <boost/operators.hpp>
#include <string>
#include <iostream>

struct Point2: public boost::additive<Point2, Point2> {
  float x;
  float y;

  Point2 operator+=(const Point2& o) { x += o.x; y += o.y; }
  Point2 operator-=(const Point2& o) { x -= o.x; y -= o.y; }
  Point2 operator*=(const float f) { x *= f; y *= f; }
};

#endif // Point2_h__69f46184_9958_46ee_8498_ca1ba6579272
