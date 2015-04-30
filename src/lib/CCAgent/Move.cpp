//Move.cpp

#include "CCAgent/Move.h"

std::unordered_map<std::pair<unsigned,unsigned>,double,pairhash> Move::hhtable;

Move::operator std::string() const {
  std::stringstream ss;
  ss << "MOVE FROM " << from << " TO " << to;
  return ss.str();
}

void Move::resetTable(){
	Move::hhtable.clear();
	for (unsigned i = 0; i < 81; i++)
		for (unsigned j = 0; j < 81; j++)
			hhtable[std::pair<unsigned,unsigned>(i,j)] = 0;
}

void Move::incrementNode(Move m, unsigned depth){
	hhtable[std::pair<unsigned,unsigned>(m.from, m.to)] += depth * depth;
}

bool operator==(const Move &lhs, const Move &rhs) {
  return lhs.from == rhs.from && lhs.to == rhs.to;
}

// Lexicographic
bool operator<(const Move &lhs, const Move &rhs) {
  return lhs.from < rhs.from || (!(rhs.from < lhs.from) && lhs.to < rhs.to);
}

std::ostream &operator<<(std::ostream &out, const Move &m) {
  out << "{" << m.from << " -> " << m.to << "}";
  return out;
}
