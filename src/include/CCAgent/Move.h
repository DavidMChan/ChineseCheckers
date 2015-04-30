//Move struct abstraction

#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <sstream>
#include <unordered_map>
#include <utility>
#include <iostream>

#include "Pairhash.h"

struct Move {
  unsigned from;
  unsigned to;

  operator std::string() const; //conversion operator

  static std::unordered_map<std::pair<unsigned, unsigned>,double, pairhash> hhtable;
  static void resetTable();
  static void incrementNode(Move m, unsigned depth);

};

struct HHMoveorder {
  bool operator ()(const Move& lhs, const Move& rhs) const
  {
    return Move::hhtable[std::pair<unsigned,unsigned>(lhs.from,lhs.to)] <= Move::hhtable[std::pair<unsigned,unsigned>(rhs.from, rhs.to)];
  }
};

bool operator==(const Move &lhs, const Move &rhs);
bool operator<(const Move &lhs, const Move &rhs);
std::ostream &operator<<(std::ostream &out, const Move &m);

#endif
