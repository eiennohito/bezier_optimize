#ifndef storage_h__76886b90_d31d_4944_9869_79404f0d339a
#define storage_h__76886b90_d31d_4944_9869_79404f0d339a

#include <iostream>
#include "geometry.h"

class PathElem {
public:
  virtual void save_to(basic_ostream& s) = 0;  
  virtual ~PathElem() {}
};

template<char L>
class ElemWithPt: public PathElem {
  Point2 pt_;
public:
  ElemWithPt(const ElemWithPt& pt): pt_(pt) {}
  override void save_to(basic_ostream& s) {
    s << L << " " << pt_;
  }
};

typedef ElemWithPt<'M'> MoveTo;
typedef ElemWithPt<'L'> LineTo;
typedef ElemWithPt<'T'> ReflectedPoint;

class HorizLine: public PathElem {
  Point2 pt_;
public:
  HorizLine(const ElemWithPt& pt): pt_(pt) {}
  override void save_to(basic_ostream& s) {
    s << "H " << static_cast<int>(pt_.x);
  }
};

class VertLine: public PathElem {
  Point2 pt_;
public:
  VertLine(const ElemWithPt& pt): pt_(pt) {}
  override void save_to(basic_ostream& s) {
    s << "V " << static_cast<int>(pt_.y);
  }
};

class BezierCurve: public PathElem {
  Point2 pt1_;
  Point2 pt2_;
public:
  BezierCurve(const Point2& pt1, const Point2& pt2): pt1_(pt1), pt2_(pt2) {}
  override void save_to(basic_ostream& s) {
    s << "Q " << pt1_ << " " << pt2_;
  }
};

class BezierSegment {

};


#endif // storage_h__76886b90_d31d_4944_9869_79404f0d339a
