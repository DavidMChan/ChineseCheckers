
#include "CCAgent/OpeningBook.h"

OpeningBook::OpeningBook(unsigned player, unsigned play_book) : books(3){

	if (player == 1){
		books.at(0).push_back(CCBoard::moveify(3,4));
		books.at(0).push_back(CCBoard::moveify(27,36));

		books.at(1).push_back(CCBoard::moveify(27, 28));
		books.at(1).push_back(CCBoard::moveify(3, 12));

	} else {
		books.at(0).push_back(CCBoard::moveify(77,76));
		books.at(0).push_back(CCBoard::moveify(53, 44));

		books.at(1).push_back(CCBoard::moveify(53, 52));
		books.at(1).push_back(CCBoard::moveify(77, 68));

		books.at(2).push_back(CCBoard::moveify(53, 52));
		books.at(2).push_back(CCBoard::moveify(71, 51));
		books.at(2).push_back(CCBoard::moveify(78, 42));
		books.at(2).push_back(CCBoard::moveify(80, 44));
		books.at(2).push_back(CCBoard::moveify(61, 41));
		books.at(2).push_back(CCBoard::moveify(79, 59));
		books.at(2).push_back(CCBoard::moveify(62, 60));
		books.at(2).push_back(CCBoard::moveify(70, 50));
		books.at(2).push_back(CCBoard::moveify(77, 43));
		books.at(2).push_back(CCBoard::moveify(69, 68));
	}
	book = play_book;
}

bool OpeningBook::getMove(uint16_t &next){
	if (mp < books.at(book).size()){
		next = books.at(book).at(mp);
		mp += 1;
		return true;
	} else{
		return false;
	}
}
