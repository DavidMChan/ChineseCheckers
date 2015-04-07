#include "Agent.h"

Agent::Agent(std::string n) : name(n) {
  unsigned c = 0;
  for (unsigned i = 0; i < 17; i++){
    std::vector<unsigned> row;
    if (i < 9){
      c = i;
      for (unsigned j = 0; j < i + 1; j++){
        row.push_back(c);
        c += 8;
      }
    }
    else{
      c = 8 + 9*(i-8);
      for (unsigned j = 0; j < 17 - i; j++){
        row.push_back(c);
        c += 8;
      }
    }
    rows.push_back(row);
  }
}

Move Agent::nextMove() {
  // Somehow select your next move
  std::set<Move> moves;
  state.getValidMoves(moves);
  
  Move m;
  float dist = -100;

  if (my_player == ChineseCheckersBoard::Player::PLAYER1){
    for (auto it = moves.begin(); it != moves.end(); it++){
        int start = 0, end = 0;
        for (unsigned i = 0; i < 17; i++){
          for (auto j : rows.at(i)){
            if (j == it->from)
              start = i;
            if (j == it->to)
              end = i;
          }
        }
        if (end - start > dist){
          dist = end-start;
          m = *it;
        }
        
    }
  }
  else{
    dist = 100;
    for (auto it = moves.begin(); it != moves.end(); it++){
        int start = 0, end = 0;
        for (int i = 0; i < 17; i++){
          for (auto j : rows.at(i)){
            if (j == it->from)
              start = i;
            if (j == it->to)
              end = i;
          }
        }
        if (end - start < dist){
          dist = end-start;
          m = *it;
        }
        
    }
  }
  

  return m;
}

void Agent::playGame() {
  // Identify myself
  std::cout << "#name " << name << std::endl;

  // Wait for start of game
  waitForStart();

  // Main game loop
  for (;;) {
    if (state.getPlayer() == my_player) {
      // My turn
      // Check if game is over
      if (state.gameOver()) {
        std::cerr << "I, " << name << ", have lost" << std::endl;
        switchCurrentPlayer();
        continue;
      }

      // Determine next move
      const Move m = nextMove();

      // Apply it locally
      state.applyMove(m);

      // Tell the world
      printAndRecvEcho(m);

      // It is the opponents turn
      switchCurrentPlayer();
    } else {
      // Wait for move from other player
      // Get server's next instruction
      std::string server_msg = readMsg();
      const std::vector<std::string> tokens = tokenizeMsg(server_msg);

      if (tokens.size() == 5 && tokens[0] == "MOVE") {
        // Translate to local coordinates and update our local state
        const Move m = state.translateToLocal(tokens);
        state.applyMove(m);

        // It is now my turn
        switchCurrentPlayer();
      } else if (tokens.size() == 4 && tokens[0] == "FINAL" &&
                 tokens[2] == "BEATS") {
        // Game over
        if (tokens[1] == name && tokens[3] == opp_name)
          std::cerr << "I, " << name << ", have won!" << std::endl;
        else if (tokens[3] == name && tokens[1] == opp_name)
          std::cerr << "I, " << name << ", have lost." << std::endl;
        else
          std::cerr << "Did not find expected players in FINAL command.\n"
                    << "Found '" << tokens[1] << "' and '" << tokens[3] << "'. "
                    << "Expected '" << name << "' and '" << opp_name << "'.\n"
                    << "Received message '" << server_msg << "'" << std::endl;
        break;
      } else {
        // Unknown command
        std::cerr << "Unknown command of '" << server_msg << "' from the server"
                  << std::endl;
      }
    }
  }
}

// Sends a msg to stdout and verifies that the next message to come in is it
// echoed back. This is how the server validates moves
void Agent::printAndRecvEcho(const std::string &msg) const {
  // Note the endl flushes the stream, which is necessary
  std::cout << msg << std::endl;
  const std::string echo_recv = readMsg();
  if (msg != echo_recv)
    std::cerr << "Expected echo of '" << msg << "'. Received '" << echo_recv
              << "'" << std::endl;
}

// Reads a line, up to a newline from the server
std::string Agent::readMsg() const {
  std::string msg;
  std::getline(std::cin, msg); // This is a blocking read

  // Trim white space from beginning of string
  const char *WhiteSpace = " \t\n\r\f\v";
  msg.erase(0, msg.find_first_not_of(WhiteSpace));
  // Trim white space from end of string
  msg.erase(msg.find_last_not_of(WhiteSpace) + 1);

  return msg;
}

// Tokenizes a message based upon whitespace
std::vector<std::string> Agent::tokenizeMsg(const std::string &msg) const {
  // Tokenize using whitespace as a delimiter
  std::stringstream ss(msg);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokens(begin, end);

  return tokens;
}

void Agent::waitForStart() {
  for (;;) {
    std::string response = readMsg();
    const std::vector<std::string> tokens = tokenizeMsg(response);

    if (tokens.size() == 4 && tokens[0] == "BEGIN" &&
        tokens[1] == "CHINESECHECKERS") {
      // Found BEGIN GAME message, determine if we play first
      if (tokens[2] == name) {
        // We go first!
        opp_name = tokens[3];
        my_player = ChineseCheckersBoard::Player::PLAYER1;
        std::cerr << "We're going first!" << std::endl;
        break;
      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = ChineseCheckersBoard::Player::PLAYER2;
        std::cerr << "We're going second!" << std::endl;
        break;
      } else {
        std::cerr << "Did not find '" << name
                  << "', my name, in the BEGIN command.\n"
                  << "# Found '" << tokens[2] << "' and '" << tokens[3] << "'"
                  << " as player names. Received message '" << response << "'"
                  << std::endl;
        std::cout << "#quit" << std::endl;
        std::exit(EXIT_FAILURE);
      }
    } else if (response == "DUMPSTATE") {
      std::cout << state.dumpState() << std::endl;
    } else if (tokens[0] == "LOADSTATE") {
      std::string new_state = response.substr(10);
      if (!state.loadState(new_state))
        std::cerr << "Failed to load '" << new_state << "'\n";
    } else if (response == "LISTMOVES") {
      std::set<Move> moves;
      state.getValidMoves(moves);
      for (const auto i : moves)
        std::cout << i.from << ", " << i.to << "; ";
      std::cout << std::endl;
    } else if (tokens[0] == "MOVE") {
      // Just apply the move
      const Move m = state.translateToLocal(tokens);
      if (!state.applyMove(m))
        std::cout << "Unable to apply move " << m << std::endl;
    } else if (response == "GETPLAYER") {
        std::cout << "Current player is: " << state.getPlayer() << std::endl;
    } else if (response == "SWAPPLAYER") {
        switchCurrentPlayer();
        std::cout << "Current player is: " << state.getPlayer() + 1 << std::endl;
    } else if (response == "QUIT") {
        exit(0);
    } else {
      std::cerr << "Unexpected message " << response << "\n";
    }
  }

  // Game is about to begin, restore to start state in case DUMPSTATE/LOADSTATE/LISTMOVES
  // were used
  state.reset();
}

void Agent::switchCurrentPlayer() {
  state.swapPlayer();
}

bool Agent::isValidStartGameMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 4 && tokens[0] == "BEGIN" && tokens[1] == "CHINESECHECKERS";
}

bool Agent::isValidMoveMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 5 && tokens[0] == "MOVE" && tokens[1] == "FROM" &&
    tokens[3] == "TO";
}
