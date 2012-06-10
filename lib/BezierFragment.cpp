#include "stdafx.h"
#include "BezierFragment.h"
#include "gauss_nums.h"
#include <math.h>
#include "WorkObjects.h"
#include <algorithm>
#include "geometry.h"

Point2 BezierFragment::interpolate( float t ) const
{
  Point2 p;
  p += p1 * (1 - t) * (1 - t);
  p += p2 * t * (1 - t) * 2;
  p += p3 * t * t;
  return p;
}

float BezierFragment::length(float z) const
{
  float v = 0;
  for(size_t i = 0; i < 5; ++i) {
    v += f(z, g_absciss[5][i]) * g_weight[5][i];
  }
  return v * (z / 2);
}

float BezierFragment::f( float z, float t ) const
{
  float z2 = z / 2;  
  float x = z2 * t + z2;
  auto p = (p1*2 - p2*4 + p3*2)*x - p1*2 + p2*2;
  auto val = sqrtf(dot_prod(p, p));
  return val;
}

float BezierFragment::difference( float olen, const std::vector<length_at_crd>& vec) const
{
  float total = 0;
  auto sz = vec.size();
  _ASSERT(sz < 64);
  float *absciss = g_absciss[sz], *wts = g_weight[sz];
  Point2 my_prev = interpolate(vec[0].length / olen);
  Point2 his_prev = vec[0].crd;

  std::for_each(vec.begin() + 1, vec.end(), [&](const length_at_crd& obj) {
    float t = obj.length / olen;
    auto pt = interpolate(t);
    total += fabs(cross_prod(my_prev - pt, my_prev - his_prev));
    total += fabs(cross_prod(obj.crd - his_prev, pt - obj.crd));
    my_prev = pt;
    his_prev = obj.crd;
  });
  return total / 2;
}

const Point2& BezierFragment::operator[]( size_t idx ) const
{
  switch(idx) {
  case 0: return p1;
  case 1: return p2;
  case 2: return p3;
  default: return p1;
  }
}

Point2& BezierFragment::operator[]( size_t idx )
{
  switch(idx) {
  case 0: return p1;
  case 1: return p2;
  case 2: return p3;
  default: return p1;
  }
}

float BezierFragment::sqdist( const BezierFragment& o ) const
{
  const size_t order = 3;
  float total = 0.0;
  for (int i = 0; i < order; ++i) {
    float z = g_absciss[order][i];
    float t = 0.5f + z * 0.5f;
    Point2 p1 = interpolate(t);
    Point2 p2 = o.interpolate(t);
    total += g_weight[order][i] * ::sqdist(p1, p2);
  }
  return total * 0.5f;
}

std::pair<BezierFragment, BezierFragment> BezierFragment::split( float at ) const
{
  Point2 p4 = p1 + (p2 - p1) * at;
  Point2 p5 = p2 + (p3 - p2) * at;
  Point2 p6 = p4 + (p5 - p4) * at;
  return std::make_pair(
      BezierFragment(p1, p4, p6),
      BezierFragment(p6, p5, p3)
    );
}
