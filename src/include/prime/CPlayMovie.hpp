#pragma once
#include <gctypes.h>

using EWhichMovie = int;

class CPlayMovie {
public:
  // can't hook a constructor directly, but this has the same sig
  CPlayMovie *CPlayMovieConstructor(EWhichMovie movie);
};

class CMoviePlayer {
public:
  CMoviePlayer *CMoviePlayerConstructor(const char *path, float preloadSeconds, bool loop, bool deinterlace);
};
