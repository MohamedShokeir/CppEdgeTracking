#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>

template <typename T> class Interpolate {
public:
  Interpolate() {}
  std::vector<T> LinearInterpolation(const std::vector<T> &vec, std::size_t k);
};

#endif