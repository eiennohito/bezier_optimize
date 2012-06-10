#include "stdafx.h"
#include "geometry.h"
#include "BezierFragment.h"

class BezuerTest: public testing::Test {};

TEST_F(BezuerTest, Test1) {
  Point2 pt(1.2f, 3.4f);
}

TEST_F(BezuerTest, TestLength) {
  Point2 p1(170, 20);
  Point2 p2(205, 210);
  Point2 p3(20, 175);
  BezierFragment fr(p1, p2, p3);

  float f = fr.length();
  EXPECT_NEAR(277.332f, f, 0.01);
}