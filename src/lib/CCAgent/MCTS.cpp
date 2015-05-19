//Implemntation file for monte carlo tree search

#include "CCAgent/MCTS.h"

/**
 * Implemntation details for the MCTS Node
 */

//std::mutex MCTS::output_lock;

MCTSNode::MCTSNode(const CCBoard &state) : 
			move(0),
			parent(nullptr),
			player(state.getPlayer()),
			node_value(0),
			visits(1),
			UCTscore(0),
			moves(0)
{
	state.getMovesNotBackwards(moves);
	std::random_shuffle(moves.begin(), moves.end());
}


MCTSNode::MCTSNode(const CCBoard& state, uint16_t move_, MCTSNode* parent_) :
			move(move_),
			parent(parent_),
			player(state.getPlayer()),
			node_value(0),
			visits(1),
			moves(0),
			UCTscore(0)
{
	state.getMovesNotBackwards(moves);
	std::random_shuffle(moves.begin(), moves.end());
}


MCTSNode::~MCTSNode(){
	for (auto child : children){
		delete child;
	}
}

bool MCTSNode::hasUnexploredMoves() const {
	return !moves.empty();
}

uint16_t MCTSNode::getUnexploredMove(std::mt19937 &gen) const {
	size_t loc = gen() % moves.size();
	return moves[loc];
}


bool MCTSNode::hasChildren() const {
	return !children.empty();
}

MCTSNode* MCTSNode::selectChild(std::mt19937 &gen, bool max) const {
	
	MCTSNode* best_child;
	double best_score = max ? -1000 : 1000;
	double temp_score = 0;

	for (auto child: children) {

		temp_score = double(child->node_value) / double(child->visits) +
			std::sqrt(FLAGS_MCTSExplore * std::log(double(this->visits)) / child->visits);
	
		if (max){
			if (temp_score > best_score){
				best_child = child;
				best_score = temp_score;
			}	
		} else {
			if (temp_score < best_score){
				best_child = child;
				best_score = temp_score;
			}
		}
	}

	return best_child;
}

MCTSNode* MCTSNode::addChild(uint16_t m, const CCBoard &state){
	auto node = new MCTSNode(state, m, this);
	children.push_back(node);
	moves.erase(std::find(moves.begin(), moves.end(), m));
	return node;
}

void MCTSNode::updateValue(double value){
	visits++;
	node_value += value;
}


/**
 * Implementation details for the MCTS agent move finder
 */

void MCTS::compute_mcts_tree(const CCBoard root_state, bool &time, TDEval eval, MCTSNode* n_out) {
	int player = root_state.getPlayer();
	unsigned iters = 0;
	unsigned seed = rand();
	std::mt19937 re(seed);

	if (FLAGS_v){
		MCTS::output_lock.lock();
		std::cerr << "[Began playouts on thread " << std::this_thread::get_id() << "]" << std::endl;
		MCTS::output_lock.unlock();
	}

	while (!time){
		iters++;
		auto node = n_out;
		CCBoard state = root_state;

		//Play the tree policy
		while (!node->hasUnexploredMoves() && node->hasChildren()) {
			node = node->selectChild(re, true);
			state.applyMove(node->move);
		}

		//If we reach a leaf, we should expand that leaf
		if (node->hasUnexploredMoves() && node->visits >= FLAGS_MCTSVisitLimit) {
			auto move = node->getUnexploredMove(re);
			state.applyMove(move);
			node = node->addChild(move, state);
		}


		double value = playOut(state, eval, player, re);
		
		//Back propagate information back through the tree
		while (node != nullptr){
			node->updateValue(value);
			node = node->parent;
		}


	}

	if (FLAGS_v){
		MCTS::output_lock.lock();
		std::cerr << "[Finished playouts on thread " << std::this_thread::get_id() << " - did " << iters << " iterations]" << std::endl;
		MCTS::output_lock.unlock();
	}
}




uint16_t MCTS::getMove(CCBoard &state, double time_limit, TDEval &eval, unsigned num_threads) {

	//Start the timer
	bool time = false;
	std::thread *time_thread = new std::thread([&time, &time_limit](){
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((time_limit-0.01)*1000))); time = true;
	});

	std::thread** threads = new std::thread*[num_threads];
	std::vector<MCTSNode*> roots;

	for (size_t nt = 0; nt < num_threads; ++nt){
		roots.push_back(new MCTSNode(state));
		threads[nt] = new std::thread(MCTS::compute_mcts_tree, state, std::ref(time), eval, roots[nt]);
	}


	//Clean up the time thread
	time_thread->join();
	delete time_thread;


	for (size_t i = 0; i < num_threads; i++){
		threads[i]->join();
		delete threads[i];
	}
	delete threads;

	//Merge the children of all of the root nodes
	std::unordered_map<uint16_t, uint64_t> visits;
	std::unordered_map<uint16_t, double> values;
	uint64_t games_played = 0;
	for (size_t t = 0; t < num_threads; t++){
		games_played += roots[t]->visits;
		for (auto child : roots[t]->children) {
			visits[child->move] += child->visits;
			values[child->move] += child->node_value;
		}
	}

	if (FLAGS_v){
		std::cerr << "Total games played: " << games_played << std::endl;
	}


	//Clean up the nodes
	for (auto i : roots){
		delete i;
	}

	//Get the move that we should make
	uint64_t best_visits = 0;
	uint16_t best_move = 0;
	
	uint64_t sum_visits = 0;

	for (auto itr : visits){
		sum_visits += itr.second;
		if (itr.second > best_visits) {
			best_visits = itr.second;
			best_move = itr.first;
			
		}
		if (FLAGS_v){
				unsigned from, to;
				CCBoard::unmovify(itr.first, from, to);
				std::cerr << "{" << from << "->" << to << ": " << itr.second << " , " << double(values[itr.first]) / double(itr.second) << "} " << std::endl;
		}
	}
	if (FLAGS_v){
		std::cerr <<std::endl << "Total visits: " << sum_visits << std::endl;
	}

	
	if (FLAGS_v){
		std::cerr << std::endl;
		double best_value = values[best_move];
		std::cerr << "Best: " << best_move
		     << " (" << 100.0 * best_visits / double(games_played) << "% visits)"
		     << " (" << double(best_value) / double(best_visits) << " average value)" << std::endl;
		std::cerr << "----" << std::endl;
	}

	if (FLAGS_v){
		std::cerr << "Total games played: " << games_played << std::endl;
		std::cerr << "-----------------------------------------------------------" << std::endl;
	}

	return best_move;
} 



double MCTS::playOut(CCBoard &state, TDEval &eval, int player, std::mt19937 &gen){
	CCBoard board(state);
	std::vector<uint16_t> mv_c;

	//Setup the cached variables that we're going to be using during the playout
	unsigned epsilon = FLAGS_MCTSEpsilon; //For now, we have an epsilon greedy factor of 10%
	uint16_t temp_best_move = 0;
	int temp_longest  = 0;
	int temp_distance = 0;
	unsigned from = 0;
	unsigned to = 0;
	unsigned depth = 0;

	//Perform the playout
	while (depth <= FLAGS_MCTSPlayoutDepth && !board.gameOver()){

		//Make a random move based on epsilon
		if (gen() % epsilon == 0){
			board.applyMove(board.getRandomMove());
		} else {
			//Or get the moves
			board.getMovesForward(mv_c);

			//Randomly shuffle them to deal with ties (for the end-game mostly)
			std::shuffle(mv_c.begin(), mv_c.end(), gen);

			//Reset the distance  variables
			temp_longest = 0;
			temp_distance = 0;
			
			//Calculate the longest possible move
			for (auto m : mv_c){
				CCBoard::unmovify(m, from, to);
				if (board.getPlayer() == 1){
					temp_distance = (to / 9+ to % 9) - (from / 9 + from % 9);
				} else {
					temp_distance = -((to / 9 + to % 9) - (from / 9 + from % 9));
				}
				if (temp_distance > temp_longest){
					temp_longest = temp_distance;
					temp_best_move = m;
				}
			}

			//Apply that move to the game
			board.applyMove(temp_best_move);
		}

		depth++;
	}

	return eval.eval(board, player);
}