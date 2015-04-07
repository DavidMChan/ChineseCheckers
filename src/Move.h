//Move struct abstraction

#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <sstream>
#include <iostream>

struct Move {
  unsigned from;
  unsigned to;

  operator std::string() const; //conversion operator
};

bool operator==(const Move &lhs, const Move &rhs);
bool operator<(const Move &lhs, const Move &rhs);
std::ostream &operator<<(std::ostream &out, const Move &m);

#endif
