#include "stdafx.h"
#include "parser.h"
#include <boost/spirit/home/qi.hpp>
#include <boost/fusion/adapted.hpp>
namespace q = boost::spirit::qi;

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

bool parse_string( const std::string& val, std::vector<BezierFragment>& frags )
{
  bf_parser<std::string::const_iterator> parser;
  return q::parse(val.begin(), val.end(), parser, frags);
}

bool parse_strings(const std::string& val, std::vector<std::vector<BezierFragment> >& frags) {
  bf_parser<std::string::const_iterator> parser;  
  return q::parse(val.begin(), val.end(), parser % "\n", frags);
}
