#ifndef BezierFragment_h__0f13a1b8_d9f0_4a09_9d72_43b71ac35ce3
#define BezierFragment_h__0f13a1b8_d9f0_4a09_9d72_43b71ac35ce3

#include "geometry.h"
#include <vector>
#include <memory>

struct length_at_crd;

#pragma once
class BezierFragment
{
public:
  Point2 p1;
  Point2 p2;
  Point2 p3;
public:
  BezierFragment(): p1(), p2(), p3() {}
  BezierFragment(const Point2& pt1, const Point2& pt2, const Point2& pt3) :
      p1(pt1), p2(pt2), p3(pt3) {}
  BezierFragment(const Point2 pts[3]): p1(pts[0]), p2(pts[2]), p3(pts[3]) {}

  Point2 interpolate(float t) const;

  float length(float z = 1.0f) const;

  float difference(float olen, const std::vector<length_at_crd>& vec) const ;

  float sqdist(const BezierFragment& o) const;
  float area( const BezierFragment& s2 ) const;

  std::pair<BezierFragment, BezierFragment> split(float at) const;

  const Point2& operator[](size_t idx) const;
  Point2& operator[](size_t idx);


private:
  float f(float z, float t) const;
  
};

typedef const BezierFragment& cbfref;
#endif // BezierFragment_h__0f13a1b8_d9f0_4a09_9d72_43b71ac35ce3

