#pragma once

#include "rstl/rstl.h"
#include "rstl/vector.h"
#include "rstl/list.h"
#include "rstl/pair.h"

RSTL_BEGIN

template < typename K, typename P, int unk, typename Select, typename Hash, typename Equal >
class hash_table {
private:
  rstl::vector< rstl::list< P > > x;
};

template < typename K, typename V, typename Hash, typename Equal >
class hash_map {
  typedef rstl::pair< K, V > Pair;

private:
  hash_table< K, Pair, 0, select1st< Pair >, Hash, Equal > table;
};

RSTL_END