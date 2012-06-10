#include "stdafx.h"
#include "geometry.h"
#include <boost/fusion/adapted.hpp>

namespace q = boost::spirit::qi;

class ParseTest: public testing::Test {};

BOOST_FUSION_ADAPT_STRUCT (
  Point2,
  (float, x)
  (float, y)
)

template<typename Iter>
struct pt_parser: q::grammar<Iter, Point2()> {
  pt_parser() : pt_parser::base_type(start) {    
    start = q::float_ >> (q::lit(' ') | q::lit(',')) >> q::float_;
  }

  q::rule<Iter, Point2()> start;
};


template<typename Iter>
bool parseLine(Iter begin, Iter end, std::vector<BezierFragment>& fv) {
  pt_parser<Iter> ptp;

}

TEST_F(ParseTest, TestDouble) {
  float val = 0;
  auto flt = q::float_;
  std::string s("2.12341");
  
  q::parse(s.begin(), s.end(), flt);
  EXPECT_EQ(2.12341f, val);
}

TEST_F(ParseTest, TestPoint2) {
  std::string str("2.123,23.14");
  pt_parser<std::string::const_iterator> psr;
  Point2 pt;
  EXPECT_TRUE(q::parse(str.begin(), str.end(), psr, pt));
  EXPECT_NEAR(2.123f, pt.x, 1e-5);
  EXPECT_NEAR(23.14f, pt.y, 1e-5);
}