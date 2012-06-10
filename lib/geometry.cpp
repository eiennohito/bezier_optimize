#include "stdafx.h"
#include "geometry.h"
#include <array>

float dot_prod( const Vector2& v1, const Vector2& v2 )
{
  return v1.x * v2.x + v1.y * v2.y;
}

float sqdist( const Point2& p1, const Point2& p2 )
{
  auto p = p2 - p1;
  return dot_prod(p, p);
}

float cross_prod( const Vector2& v1, const Vector2& v2 )
{
  return v1.x * v2.y - v2.x * v1.y;
}
