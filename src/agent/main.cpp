//Main - used for generating the binaries that we need

#include <sstream>
#include <thread>
#include <iostream>

#include "CCAgent/Agent.h"
#include "gflags/gflags.h"


DEFINE_string(name, "Serenity",
                 "The name that the agent should play under");
DEFINE_bool(v, false, "Verbose output");
DEFINE_bool(v_ab, false, "Verbose alpha-beta output");
DEFINE_bool(dont_prune, false, "Remove pruning from alpha beta (basically only for debug)");
DEFINE_bool(v_t, false, "Verbose transposition output");
DEFINE_double(time_limit, 9.8, "The amount of time that the agent should think before selecting a move");
DEFINE_int32(random_seed, 0, "A seed for the random number generator");
DEFINE_bool(t, true, "Use transposition tables");
DEFINE_bool(ob, true, "Use opening book");
DEFINE_string(weights, "none", "Weight file to load regression weights from");

void launchAgent(std::string, std::istream&, std::ostream&);
void launchAgent(std::string name,std::istream& in, std::ostream& out){
	Agent a(name, in, out);
  	a.playGame();
}

int main(int argc, char** argv){
	gflags::SetVersionString("0.1 - 04/15/2015");
	gflags::SetUsageMessage("Firefly v0.1 command agent - based off of the version developed by Will Mitchell, and activly developed by David Chan");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	srand (static_cast<unsigned int>(FLAGS_random_seed));

	std::thread* t = new std::thread(launchAgent, FLAGS_name, std::ref(std::cin), std::ref(std::cout));
  	t->join();

  	return EXIT_SUCCESS;
}
