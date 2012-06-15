#ifndef WorkObjects_h__d11d954d_03e8_4e4a_a6ae_03d9d417053a
#define WorkObjects_h__d11d954d_03e8_4e4a_a6ae_03d9d417053a

#include <vector>
#include <memory>
#include <algorithm>
#include "BezierFragment.h"

struct length_at_crd {
  Point2 crd;
  float length;
};

class bezier_chain {
  const std::vector<BezierFragment> &stor_;
  size_t begin_;
  size_t end_;
  bezier_chain(const std::vector<BezierFragment>& st, size_t beg, size_t end): stor_(st), begin_(beg), end_(end) {}
  friend class chain_storage;
public:

  bezier_chain(const bezier_chain& o): stor_(o.stor_), begin_(o.begin_), end_(o.end_) {}

  size_t size() const { return end_ - begin_; }
  size_t first() const { return begin_; }
  size_t last() const { return end_; }

  const BezierFragment& at(size_t idx) const { return stor_[begin_ + idx]; }
  const Point2& tip() const { return at(0).p1; }
  const Point2& tail() const { return at(size() - 1).p3; }

  std::pair<bezier_chain, bezier_chain> split(size_t at) const {
    std::make_pair(bezier_chain(stor_, begin_, at), bezier_chain(stor_, at, end_));
  }

  template<typename F>
  F for_each_fragment(F f) const {
    return for_each(stor_.begin() + begin_, stor_.begin() + end_, f);
  }

  float sqdist(const bezier_chain& o, std::vector<float>& opts, std::vector<float>& buf) const;
  float area(const bezier_chain& o, std::vector<float>& opts, std::vector<float>& buf) const;
  float dist(const bezier_chain& o, std::vector<float>& v1, std::vector<float>& v2) const;
  float length() const;
  float calc_begins(std::vector<float>& buf) const;

  float length_at_crds(int cnt, std::vector<length_at_crd>& lacs) const;
  BezierFragment head() const;
  BezierFragment crude_appx() const;
  int gd_appx(chain_storage& o, std::vector<float>& v1, std::vector<float>& v2) const;
  int max_angle() const;
  int simplify_gd(chain_storage& st, float margin, std::vector<float>& v1, std::vector<float>& v2) const;
  //split-simplify-merge
  int ssm_gd(int at, chain_storage& st, float margin, std::vector<float>& v1, std::vector<float>& v2) const;
};

class chain_storage {
  std::vector<BezierFragment> lines_;
public:
  void clear() { lines_.clear(); }
  size_t size() const { return lines_.size(); }
  bezier_chain chain() const;
  const Point2& tail() const;
  const BezierFragment& at(size_t idx) const { return lines_[idx]; }
  BezierFragment& at(size_t idx) { return lines_[idx]; }
  void push(const BezierFragment& frag);
  void push(const bezier_chain& ch);
  std::vector<BezierFragment>& data();
};

class work_objs {

};

#endif // WorkObjects_h__d11d954d_03e8_4e4a_a6ae_03d9d417053a
