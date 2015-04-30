//Implentation of the endgame database

#include "CCAgent/EndgameDatabaseElement.h"
#include "CCAgent/SixCCS.h"
#include "CCAgent/Move.h"
#include "CCAgent/Pairhash.h"

#include <boost/multiprecision/cpp_int.hpp>



#include <vector>
#include <utility>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>

#ifndef ENDGAME_DATABASE_H_INCLUDED
#define ENDGAME_DATABASE_H_INCLUDED

class EndgameDatabase {
	public:
		EndgameDatabase(std::string filename, uint64_t num_state, uint64_t num_piece) : endgame_database(num_state/4 + 1), unrankCache(num_piece), num_states(num_state), num_pieces(num_piece)
		{
			cacheMNCs();
			if (filename == "none"){
				
				for (uint64_t i = 0; i < num_state; i++)
					set(i, 0x3F);
				//GenerateAndSave("endgame.egdb");
			} else {
				//Load the database.... this could also take a really long time
			}
			
		}
		~EndgameDatabase() {}

		//int Lookup(const ChineseCheckersState &state); //Returns the number of moves for a single agent with
													   //perfect play. Returns -1 if not in table
		void GenerateAndSave(std::string filename);
	private:
		void set(uint64_t location, unsigned short value);
		unsigned short fetch(uint64_t location);

		uint64_t binom(uint64_t a, uint64_t b);
		void cacheMNCs();

		uint64_t rankState();
		void unrankState(uint64_t rank);

		//bool saveDatabaseToFile(std::string filename);
		//bool loadDatabaseFromFile(std::string filename);
		std::vector<unsigned> unrankCache;
		std::unordered_map<std::pair<unsigned, unsigned>, uint64_t, pairhash> multinomial_coeffs;
		std::vector<DBElement> endgame_database;
		uint64_t num_states;
		uint64_t num_pieces;
		SixCCS state;
};

#endif
