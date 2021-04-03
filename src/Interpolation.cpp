#include "Interpolation.h"

#include <cmath>
#include <numeric>
#include <vector>

// template <typename T>
// std::vector<T> Interpolate<T>::LinearInterpolation(const std::vector<T> &vec,
//                                                    std::size_t k) {
//   if (vec.empty())
//     return {};

//   std::vector<T> res(vec.size() * (k - 1) + 1);

//   for (std::size_t i = 0; i + 1 < vec.size(); ++i) {
//     for (std::size_t j = 0; j != k; ++j) {
//       res[i * k + j] = std::lerp(vec[i], vec[i + 1], float(j) / k);
//     }
//   }
//   res.back() = vec.back();
//   return res;
// }