//Implementation file for the endgame database

#include "CCAgent/EndgameDatabase.h"

uint64_t EndgameDatabase::binom(uint64_t a, uint64_t b)
{

	if (a <= b)
		return 1;

	boost::multiprecision::uint256_t ret = 1;
    for(boost::multiprecision::uint256_t i = a; i > 1; --i){
        ret *= i;
    }

    boost::multiprecision::uint256_t val2 = 1;
    for(boost::multiprecision::uint256_t i = b; i > 1; --i){
        val2 *= i;
    }

     boost::multiprecision::uint256_t val1 = 1;
    for(boost::multiprecision::uint256_t i = (a-b); i > 1; --i){
        val1 *= i;
    }

    ret = ret / (val1 * val2);

    return static_cast<uint64_t>(ret);
}

void EndgameDatabase::cacheMNCs(){
	for (unsigned i = 0; i < 82; i++){
		for (unsigned j = 0; j < 11; j ++){
			multinomial_coeffs.insert({{i,j}, binom(i,j)});
		}
	}
}


uint64_t EndgameDatabase::rankState(){
	std::vector<unsigned> vals(state.p1Loc.begin(), state.p1Loc.end());
	std::sort(vals.begin(), vals.end());

	uint64_t r = 0;
	unsigned short t = 0;
	unsigned short v = vals.size();
	for (int i = 0; i < 81; i++){
		if (vals[t] == i){
			t++;
			v--;
		}
		else
			r += multinomial_coeffs[{81 - i,v - 1}];
	}
	return r;
}

void EndgameDatabase::unrankState(uint64_t rank){
	unrankCache.clear();
	uint64_t r = rank;
	unsigned np = num_pieces;
	for (unsigned i = 0; i < 81; i++){
		if (r < multinomial_coeffs[{81 - i - 1, np - 1}]){
			unrankCache.push_back(i);
			np -= 1;
		} else {
			r -= multinomial_coeffs[{81 - i - 1, np - 1}];
		}
	}
	state.reset(unrankCache);
}


unsigned short EndgameDatabase::fetch(uint64_t location){
	//First calculate the offset - there are 4 elements in each box, so
	uint64_t element_box = location / 4;
	//Now calculate the cell
	uint64_t element_cell = location % 4;
	//Now fetch the element
	switch(element_cell){
		case 0:
			return endgame_database[element_box].a;
		case 1:
			return endgame_database[element_box].b;
		case 2:
			return endgame_database[element_box].c;
		case 3:
			return endgame_database[element_box].d;
		default:
			return 0;
	}
}

void EndgameDatabase::set(uint64_t location, unsigned short value){

	//First sanitize the value
	value &= 0x3F;

	//Now calculate the offset - there are 8 elements in each box, so
	uint64_t element_box = location / 4;
	//Now calculate the cell
	uint64_t element_cell = location % 4;
	//Now fetch the element
	switch(element_cell){
		case 0:
			endgame_database[element_box].a = value;
		case 1:
			endgame_database[element_box].b = value;
		case 2:
			endgame_database[element_box].c = value;
		case 3:
			endgame_database[element_box].d = value;
		default:
			return;

	}
}

void EndgameDatabase::GenerateAndSave(std::string filename){
	
	state.reset();
	std::set<Move> moves;

	bool done = false;
	unsigned short depth = 0;
	uint64_t states_seen = 0;
	uint64_t rank;

	set(rankState(), 0);

	while (!done){
		bool flag = true;
		for (uint64_t i = 0; i < num_states; ++i){
			if (fetch(i) == depth){

				unrankState(i);
				state.getMoves(moves);

				for (Move m : moves){

					state.applyMove(m);
					rank = rankState();
					state.undoMove(m);

					if (fetch(rank) > depth){
						set(rank, depth + 1);
						states_seen += 1;
					}
				}

				flag = false;
			}
		}
		
		if (flag)
			done = true;

		depth ++;
		std::cerr << "Built endgameDB to depth " << depth << " - states seen: " << states_seen << std::endl;
	}


	std::cerr << "Finished building database" << std::endl;
	std::ofstream out(filename);
	for (uint64_t i = 0; i < num_states; ++i){
		out << (char)fetch(i) << std::endl;;
	}
	out.close();
}


