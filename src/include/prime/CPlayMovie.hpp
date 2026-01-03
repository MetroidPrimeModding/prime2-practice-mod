#pragma once
#include <gctypes.h>

using EWhichMovie = u32;

class CPlayMovie {
public:
  // can't hook a constructor directly, but this has the same sig
  void CPlayMovieConstructor(EWhichMovie movie);
};

class CMoviePlayer {
public:
  void CMoviePlayerConstructor(char *path, double preloadSeconds, int loop, int deinterlace);
};
