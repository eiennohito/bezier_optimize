#include "stdafx.h"
#include "geometry.h"

class PointTest: public testing::Test {};

TEST_F(PointTest, TestSum) {
  Point2 p1(10, 15);
  Point2 p2(20, 30);
  Point2 p3 = p1 + p2;
  EXPECT_EQ(30, p3.x);
  EXPECT_EQ(45, p3.y);
}

TEST_F(PointTest, TestMult) {
  Point2 p1(10, 15);
  Point2 p2 = p1 * 5;
  EXPECT_EQ(50, p2.x);
  EXPECT_EQ(75, p2.y);
}

TEST_F(PointTest, TestExpr) {
  Point2 p1(10, 10);
  Point2 p2(5, 4);
  Point2 p3 = p1 + p2 * 3;
  EXPECT_EQ(25, p3.x);
  EXPECT_EQ(22, p3.y);
}

TEST_F(PointTest, TestExpr2) {
  Point2 p1(10, 10);
  Point2 p2(5, 4);
  Point2 p3 = (p1 + p2) * 3;
  EXPECT_EQ(45, p3.x);
  EXPECT_EQ(42, p3.y);
}

TEST_F(PointTest, TestBigExpr) {
  Point2 p1(170, 20);
  Point2 p2(205, 210);
  Point2 p3(20, 175);

  Point2 p = (p1*2 - p2*4 + p3*2)*0.5 - p1*2 + p2*2;
  EXPECT_EQ(-150, p.x);
  EXPECT_EQ(155, p.y);
}