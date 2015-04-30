#include <utility>

#ifndef PAIRHASH_H_INCLUDED
#define PAIRHASH_H_INCLUDED

struct pairhash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};

#endif
