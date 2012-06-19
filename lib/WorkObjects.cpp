#include "StdAfx.h"
#include "WorkObjects.h"
#include <algorithm>
#include <cmath>
#include <tuple>
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define RPT(msg, ...) _RPT_BASE((_CRT_WARN, NULL, 0, NULL, msg, __VA_ARGS__))
#else
#define RPT(msg, ...)
#endif


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

float bezier_chain::area( const bezier_chain& o, std::vector<float>& opts, std::vector<float>& pts ) const
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
    total += s1.area(s2);
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

int bezier_chain::gd_appx( chain_storage& o, std::vector<float>& v1, std::vector<float>& v2) const
{
  if (equal(tip(), tail(), 1e-5)) {
    //o.push(BezierFragment(tip(), tip(), tip()));
    return -1;
  }
  {
    o.clear();  
    BezierFragment bf(tip(), tip() + (tail() - tip()) * 0.5, tail());
    o.push(bf);
  }  
  BezierFragment& bf = o.at(0);
  bezier_chain ch(o.chain());
  float last = sqdist(ch, v1, v2);
  const int max_iter = 10;
  int iter = 0;
  for (; iter < max_iter; ++iter) {
    const float delta = 0.01f;
    auto pt = bf.p2;
    float f0 = sqdist(ch, v1, v2);
    bf.p2 = Point2(pt.x + delta, pt.y);
    float fx = sqdist(ch, v1, v2);
    bf.p2 = Point2(pt.x, pt.y + delta);
    float fy = sqdist(ch, v1, v2);
    float dx = (fx - f0) / delta;
    float dy = (fy - f0) / delta;
    Point2 np(pt.x - dx * 1.5, pt.y - dy * 1.5);
    bf.p2 = np;
    float dist = sqdist(ch, v1, v2);
    if (fabs(last - dist) < 0.001) {
      break;
    }
    last = dist;
  }
  return iter;
}

int bezier_chain::max_angle() const
{
  if (size() <= 2) {
    return -1;
  }
  int min = size() - 1;
  float mina = dot_prod(at(0).p2 - at(0).p1, at(min).p2 - at(min).p3);
  for (int i = 0; i < size() - 1; ++i) {
    float val = dot_prod(at(i).p2 - at(i).p3, at(i + 1).p2 - at(i + 1).p1);
    if (val < mina) {
      mina = val;
      min = i;
    }
  }
  return min;
}

simplify_result bezier_chain::simplify_gd_inner( chain_storage& st, float margin, std::vector<float>& v1, std::vector<float>& v2) const
{
  float mylen = length();
  int sz = size();
  if (sz == 1) {
    st.push(at(0));
    simplify_result res = {1, 0, 0};
    return res;
  }
  chain_storage temp;  
  if (sz == 2) {
    if (gd_appx(temp, v1, v2) != -1) {
      auto error = temp.chain().sqdist(*this, v1, v2);
      if (error < margin) {
        st.push(temp.at(0));
        simplify_result res = {1, 1, error};
        return res;
      } else {
        st.push(*this);
        simplify_result res = {2, 0, 0};
        return res;
      }
    }
  }
  if (sz == 3) {
    if (gd_appx(temp, v1, v2) != -1) {
      auto error = temp.chain().sqdist(*this, v1, v2);
      if (error < margin) {
        st.push(temp.at(0));
        simplify_result res = {1, 2, error};
        return res;
      } else {
        chain_storage left, right;        
        simplify_result lc = to(2).simplify_gd_inner(left, margin, v1, v2);
        simplify_result rc = from(1).simplify_gd_inner(right, margin, v1, v2);
        if (left.size() < right.size()) {
          st.push(left.chain());
          st.push(at(2));
          simplify_result res = {lc.result_pts + 1, 2 - lc.result_pts, lc.error};
          return res;
        } else {          
          st.push(at(0));
          st.push(right.chain());
          simplify_result res = {rc.result_pts + 1, 2 - rc.result_pts, rc.error};
          return res;
        }
      }
    }
  }

  if (sz < 64) {
    if (gd_appx(temp, v1, v2) != -1) {
      auto error = temp.chain().sqdist(*this, v1, v2);
      if (error < margin) {
        st.push(temp.at(0));
        simplify_result res = {1, sz - 1, error};
        return res;
      }
    }
  }
  chain_storage stor[3];
  simplify_result pos[3];
  int splits[3] = { sz / 3,
    sz / 2,
    2 * sz / 3};
  for (int i = 0; i < 3; ++i) {
    pos[i] = ssm_gd(splits[i], stor[i], margin, v1, v2);
  }
  simplify_result* minel = std::min_element(pos, pos + 3, [](const simplify_result& sr1, const simplify_result& sr2) {
    return (sr1.error / sr1.reduce) < (sr2.error / sr2.reduce);
  });
  st.push(stor[minel - pos].chain());
  return *minel;
}

simplify_result bezier_chain::ssm_gd( int at, chain_storage& st, float margin, std::vector<float>& v1, std::vector<float>& v2 ) const
{  
  _RPTF4(_CRT_WARN, "Splitting %d line [%d, %d] at %d\n", size(), begin_, end_, at);
  auto chns = split(at);  
  float fs = chns.first.size();
  float ss = chns.second.size();
  float total = fs * fs + ss * ss;
  float m1 = margin * fs / size();
  float m2 = margin * ss / size();

  chain_storage temp;

  simplify_result s1 = chns.first.simplify_gd_inner(temp, margin / 2, v1, v2);
  simplify_result s2 = chns.second.simplify_gd_inner(temp, margin / 2, v1, v2);
  if (temp.size() <= 3) {
    chain_storage temp2;
    auto ssize = temp.chain().simplify_gd_inner(temp2, margin, v1, v2);
    auto error = sqdist(temp2.chain(), v1, v2);
    if (error < margin) {
      _RPTF4(_CRT_WARN, "Splitting-merging-simplfying of %d line [%d, %d] -> %d\n", size(), begin_, end_, ssize.result_pts);
      st.push(temp2.chain());
      return ssize;
    }
  } 
  st.push(temp.chain());
  RPT("Splitted and merged %d line [%d, %d] to %d, %f and %d, %f; margin = %f\n", size(), begin_, end_, s1.result_pts, s1.error, s2.result_pts, s2.error, margin);
  simplify_result res = { s1.result_pts + s2.result_pts, s1.reduce + s2.reduce, s1.error + s2.error };
  return res;
}

float bezier_chain::length() const
{
  float len = 0;
  const auto sz = size();
  for (int i = 0; i < sz; ++i) {
    len += at(i).length();
  }
  return len;
}

float bezier_chain::dist( const bezier_chain& o, std::vector<float>& v1, std::vector<float>& v2 ) const
{
  float ar = o.area(*this, v1, v2);
  float len = o.length() + length();
  return 2 * ar / len;
}

simplify_result bezier_chain::simplify_gd( chain_storage& st, float margin, std::vector<float>& v1, std::vector<float>& v2 ) const
{
  float xmin = 1e10f;
  float xmax = -1e10f;
  float ymin = 1e10f;
  float ymax = -1e10f;
  for_each_fragment([&](const BezierFragment& f) {
    for (int i = 0; i < 3; ++i) {
      xmin = std::min(f[i].x, xmin);
      xmax = std::max(f[i].x, xmax);
      ymin = std::min(f[i].y, ymin);
      ymax = std::max(f[i].y, ymax);
    }
  });
  float m = (xmax - xmin) * (ymax - ymin) * margin / 100.f;
  return simplify_gd_inner(st, m, v1, v2);
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

void chain_storage::push( const bezier_chain& ch )
{
  ch.for_each_fragment([this](const BezierFragment& bf) { 
    this->push(bf);
  });
}

std::vector<BezierFragment>& chain_storage::data()
{
  return lines_;
}
