//Opening Book

#ifndef OPENING_BOOK_H_INCLUDED
#define OPENING_BOOK_H_INCLUDED

#include "CCAgent/CCBitboard.h"
#include <vector>

class OpeningBook {
	public:
		OpeningBook(unsigned player, unsigned book = 0);
		bool getMove(uint16_t &next);
	private:
		std::vector<std::vector<uint16_t>> books;
		unsigned book;
		unsigned mp = 0;
};

#endif
