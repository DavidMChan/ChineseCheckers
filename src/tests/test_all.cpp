#include "gtest/gtest.h"
#include "gflags/gflags.h"

#define DEBUG

DEFINE_string(name, "Serenity",
                 "The name that the agent should play under");
DEFINE_bool(v, false, "Verbose output");
DEFINE_bool(v_ab, false, "Verbose alpha-beta output");
DEFINE_bool(dont_prune, false, "Remove pruning from alpha beta (basically only for debug)");
DEFINE_bool(v_t, false, "Verbose transposition output");
DEFINE_double(time_limit, 9.8, "The amount of time that the agent should think before selecting a move");
DEFINE_int32(random_seed, 0, "A seed for the random number generator");
DEFINE_int32(depth, 3, "The depth to explore the tree in the test");
DEFINE_int32(eval, 0, "The base eval method to use");
DEFINE_bool(t, true, "Use transposition tables");
DEFINE_bool(ob, false, "Use opening book");
DEFINE_string(weights, "testweights.regweights", "The weights to use for the regression");

int main(int argc, char **argv) {
  gflags::SetVersionString("0.1 - 04/15/2015");
  gflags::SetUsageMessage("Firefly v0.1 command agent - based off of the version developed by Will Mitchell, and activly developed by David Chan");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
