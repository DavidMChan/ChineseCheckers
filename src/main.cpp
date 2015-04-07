#include <cstddef>
#include <ctime>
#include "Agent.h"

int main(int argc, char **argv) {
  srand (time(NULL));
  Agent a(argc >= 2 ? argv[1] : "Serenity");
  a.playGame();
  return EXIT_SUCCESS;
}
