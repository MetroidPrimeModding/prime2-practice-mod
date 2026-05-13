#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

template <typename A, typename B> class PACKED pair {
public:
  A a;
  B b;
};

template < typename P >
struct select1st {
  const P& operator()(const P& it) const { return it; }
};

template < typename K, typename V >
struct select1st< pair< K, V > > {
  typedef K value_type;

  const K& operator()(const pair< K, V >& it) const { return it.a; }
};

RSTL_END
