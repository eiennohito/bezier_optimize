#include "StdAfx.h"
#include "WorkObjects.h"
#include <algorithm>
#include <cmath>

float round(float r) {
  return (r > 0.0f) ? std::floor(r + 0.5f) : std::ceil(r - 0.5f);
}

const float val[] = {1.2f, 3.5f, 7.6f};
const float val2[] = {1.2f, 3.5f};
const float *vala[] = {val, val2};

float bezier_chain::length_at_crds( int cnt, std::vector<length_at_crd>& lacs ) const
{
  float total = 0;
  bool fst = true;
  float ratio = 1.0f / cnt;
  for_each_fragment([&](const BezierFragment& f) {
    if (fst) {
      length_at_crd l = {f.p1, 0.f};
      lacs.push_back(l);
      fst = false;
    }
    float len = f.length();
    total += len;
    for (int i = 1; i <= cnt; ++i) { //up to cnt - 1      
      float r = ratio * i;
      length_at_crd l = {f.interpolate(r), r * len};
      lacs.push_back(l);      
    }    
  });
  return total;
}

BezierFragment bezier_chain::head() const
{
  return stor_[begin_];
}

//{{R -> 0.84375 P1 + 0.875 P2 + 0.5 P3 - 1.21875 P5}}
BezierFragment bezier_chain::crude_appx() const
{
  if (size() == 1) {
    return head();
  }
  const Point2& P1 = tip();
  const Point2& P5 = tail();
  float sz = 2*size() + 1;
  auto pt = sz / 4;
  size_t c2 = static_cast<size_t>(round(pt));
  size_t c3 = static_cast<size_t>(round(pt * 2));
  size_t c2i = c2 / 3, c2p = c2 % 3;
  size_t c3i = c3 / 3, c3p = c3 % 3;
  const Point2& P2 = at(c2i)[c2p];
  const Point2& P3 = at(c3i)[c3p];
  return BezierFragment(
      P1,
      P1 * 0.462507f + P2 * 1.38332f + P3 * 1.28203f - P5 * 2.12787f,
      P5
    );
}

float bezier_chain::sqdist( const bezier_chain& o, std::vector<float>& opts, std::vector<float>& pts ) const
{
  if (size() == 0 || o.size() == 0) { return 0; }
  opts.clear(); o.calc_begins(opts);
  pts.clear(); this->calc_begins(pts);

  float total = 0;
  size_t c1 = 1, c2 = 1;
  BezierFragment f1 = at(0), f2 = o.at(0);
  float e1 = pts[0], e2 = opts[0];
  do {
    BezierFragment s1, s2;
    if (e1 < e2) { // need to cut other segment
      s1 = f1;      
      auto sgs = f2.split(e1);
      s2 = sgs.first;
      f2 = sgs.second;
      if (c1 != size()) {
        f1 = at(c1);
        e1 = pts[c1];
        ++c1;
      }
    } else {
      s2 = f2;
      auto sgs = f1.split(e2);
      s1 = sgs.first;
      f1 = sgs.second;
      if (c2 != o.size()) {
        f2 = o.at(c2);
        e2 = opts[c2];
        ++c2;
      }
    }
    total += s1.sqdist(s2);
  } while (c1 != size() || c2 != o.size());
  return total;
}

float bezier_chain::calc_begins( std::vector<float>& buf ) const
{
  float total = 0;
  for_each_fragment([&total, &buf](const BezierFragment& f) {
    total += f.length();
    buf.push_back(total);
  });
  std::for_each(buf.begin(), buf.end(), [&](float& o) {
    o /= total;
  });
  return total;
}

int bezier_chain::gd_appx( chain_storage& o ) const
{
  {
    o.clear();  
    BezierFragment bf(tip(), tip() + (tail() - tip()) * 0.5, tail());
    o.push(bf);
  }
  std::vector<float> v1, v2;
  BezierFragment& bf = o.at(0);
  bezier_chain ch(o.chain());
  float last = sqdist(ch, v1, v2);
  int iter = 0;
  for (; iter < 50; ++iter) {
    const float delta = 0.01f;
    auto pt = bf.p2;
    float f0 = sqdist(ch, v1, v2);
    bf.p2 = Point2(pt.x + delta, pt.y);
    float fx = sqdist(ch, v1, v2);
    bf.p2 = Point2(pt.x, pt.y + delta);
    float fy = sqdist(ch, v1, v2);
    float dx = (fx - f0) / delta;
    float dy = (fy - f0) / delta;
    Point2 np(pt.x - dx, pt.y - dy);
    bf.p2 = np;
    float dist = sqdist(ch, v1, v2);
    if (fabs(last - dist) < 0.001) {
      break;
    }
    last = dist;
  }
  return iter;
}

bezier_chain chain_storage::chain() const
{
  return bezier_chain(lines_, 0, lines_.size());
}

const Point2& chain_storage::tail() const
{
  return lines_.back().p3;
}

void chain_storage::push( const BezierFragment& frag )
{
  lines_.push_back(frag);
}
