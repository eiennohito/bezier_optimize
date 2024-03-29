#include "stdafx.h"
#include "geometry.h"
#include "BezierFragment.h"
#include <boost/fusion/adapted.hpp>

namespace q = boost::spirit::qi;

class ParseTest: public testing::Test {};

BOOST_FUSION_ADAPT_STRUCT (
  Point2,
  (float, x)
  (float, y)
)


BOOST_FUSION_ADAPT_STRUCT (
  BezierFragment, 
  (Point2, p1)
  (Point2, p2)
  (Point2, p3)
)

template<typename Iter>
struct pt_parser: q::grammar<Iter, Point2()> {
  pt_parser() : pt_parser::base_type(start) {    
    start = q::float_ >> (q::lit(' ') | q::lit(',')) >> q::float_;
  }

  q::rule<Iter, Point2()> start;
};

template<typename Iter>
struct bf_parser: q::grammar<Iter, std::vector<BezierFragment>()> {

  bf_parser(): bf_parser::base_type(start) {
    fragment = q::lit('(')
      >> point >> ' ' 
      >> point >> ' ' 
      >> point >> ')';
    start = *fragment;
  }

  pt_parser<Iter> point;
  q::rule<Iter, BezierFragment()> fragment;
  q::rule<Iter, std::vector<BezierFragment>()> start;
};


TEST_F(ParseTest, TestDouble) {
  float val = 0;
  auto flt = q::float_;
  std::string s("2.12341");
  
  q::parse(s.begin(), s.end(), flt, val);
  EXPECT_EQ(2.12341f, val);
}

TEST_F(ParseTest, ParseFragments) {
  std::string str("(10 15 20 30 10 20)");
  bf_parser<std::string::const_iterator> parser;
  std::vector<BezierFragment> frags;
  q::parse(str.begin(), str.end(), parser, frags);
  int i = 0;
  ++i;
}

TEST_F(ParseTest, TestPoint2) {
  std::string str("2.123,23.14");
  pt_parser<std::string::const_iterator> psr;
  Point2 pt;
  EXPECT_TRUE(q::parse(str.begin(), str.end(), psr, pt));
  EXPECT_NEAR(2.123f, pt.x, 1e-5);
  EXPECT_NEAR(23.14f, pt.y, 1e-5);
}