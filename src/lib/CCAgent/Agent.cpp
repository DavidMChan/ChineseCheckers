#include "CCAgent/Agent.h"

/**
 * Select the next move. 
 * @return Move - the move that should be played next on the local state
 */
uint16_t Agent::nextMove(){

  if (FLAGS_ob){
    uint16_t m;
    if (my_book->getMove(m)){
      return m;
    } else {
      return AB::AlphaBetaRoot(state, t_eval, static_cast<float>(FLAGS_time_limit),  my_player == 1 ? 0 : 1 );
    }
  } else {
    return AB::AlphaBetaRoot(state,t_eval, static_cast<float>(FLAGS_time_limit),  my_player == 1 ? 0 : 1 );
  }
}

/**
 * Play the game - this uses the standard agent commands
 */
void Agent::playGame() {

  // Identify myself
  output << "#name " << name << std::endl;
  output.flush();

  // Wait for start of game
  if (!waitForStart())
    return;

  // Main game loop
  for (;;) {
    if (current_player == my_player) {
      // My turn
      // Check if game is over
      if (state.gameOver()) {
        switchCurrentPlayer();
        continue;
      }

      // Determine next move
      const uint16_t m = nextMove();

      // Apply it locally
      state.applyMove(m);

      // Tell the world
      unsigned f,t;
      CCBoard::unmovify(m, f, t);
      std::string msg = ("MOVE FROM " + std::to_string(f) + " TO " + std::to_string(t));
      
      printAndRecvEcho(msg);

      // It is the opponents turn
      switchCurrentPlayer();
    } else {
      // Wait for move from other player
      // Get server's next instruction
      std::string server_msg = readMsg();
      const std::vector<std::string> tokens = tokenizeMsg(server_msg);

      if (tokens.size() == 5 && tokens[0] == "MOVE") {
        // Translate to local coordinates and update our local state
        const uint16_t m = state.translateToLocal(tokens);
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
  output << msg << std::endl;
  const std::string echo_recv = readMsg();
  if (msg != echo_recv)
    std::cerr << "Expected echo of '" << msg << "'. Received '" << echo_recv
              << "'" << std::endl;
}

// Reads a line, up to a newline from the server
std::string Agent::readMsg() const {
    std::string msg;
    std::getline(input, msg);
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

bool Agent::waitForStart() {
  for (;;) {
    std::string response = readMsg();
    bool s_break;
    if (ProcessCommand(response, s_break)){
      if (s_break)
        break;
    } else {
      return false;
    }
    output.flush();
  }

  // Game is about to begin, restore to start state in case DUMPSTATE/LOADSTATE/LISTMOVES
  // were used
  delete my_book;
  my_book = new OpeningBook(my_player, rand() % 3);

  state.reset();
  return true;
}

void Agent::switchCurrentPlayer() {
  current_player = 3 - current_player;
}

bool Agent::isValidStartGameMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 4 && tokens[0] == "BEGIN" && tokens[1] == "CHINESECHECKERS";
}

bool Agent::isValidMoveMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 5 && tokens[0] == "MOVE" && tokens[1] == "FROM" &&
    tokens[3] == "TO";
}

int Agent::GetMyPlayer(){
  return my_player;
}

//Only used externally - so we're going to co-opt it to initialize variables
bool Agent::ProcessCommand(std::string response, bool &suggestBreak, std::stringstream &out){
  const std::vector<std::string> tokens = tokenizeMsg(response);
  suggestBreak = false;
  if (tokens.size() == 4 && tokens[0] == "BEGIN" &&
        tokens[1] == "CHINESECHECKERS") {
      // Found BEGIN GAME message, determine if we play first
      current_player = 1;
      if (tokens[2] == name) {
        // We go first!
        opp_name = tokens[3];
        my_player = 1;
        std::cerr << "We're going first!" << std::endl;
        suggestBreak = true;

      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = 2;
        
        std::cerr << "We're going second!" << std::endl;
        suggestBreak = true;
      } else {
        std::cerr << "Did not find '" << name
                  << "', my name, in the BEGIN command.\n"
                  << "# Found '" << tokens[2] << "' and '" << tokens[3] << "'"
                  << " as player names. Received message '" << response << "'"
                  << std::endl;
        out << "#quit" << std::endl;
        std::exit(EXIT_FAILURE);
      }
    } else if (response == "IDENTIFY") {
      out << "#name " << name << std::endl;
    } else if (response == "DUMPSTATE") {
      out << state.dumpState() << std::endl;
    } else if (tokens[0] == "LOADSTATE") {
      std::string new_state = response.substr(10);
      if (!state.loadState(new_state))
        std::cerr << "Failed to load '" << new_state << "'\n";
    } else if (response == "LISTMOVES") {
      std::vector<uint16_t> moves;
      unsigned from, to;
      state.getMoves(moves);
      for (const auto i : moves){
        CCBoard::unmovify(i, from, to);
        out << from << ", " << to << "; ";
      }
      out << std::endl;
    } else if (tokens[0] == "MOVE") {
      // Just apply the move
      const uint16_t m = state.translateToLocal(tokens);
      if (!state.applyMove(m))
        out << "Unable to apply move " << m << std::endl;
    } else if (response == "GETPLAYER") {
        out << "Current player is: " << current_player << std::endl;
    } else if (response == "SWAPPLAYER") {
        switchCurrentPlayer();
        out << "Current player is: " << current_player << std::endl;
    } else if (response == "SWAPSTATEPLAYER") {
        state.swapPlayer();
        out << "Current state player is: " << state.getPlayer() << std::endl;
    } else if (response == "NEXTMOVE") {
      uint16_t m = nextMove();
      unsigned from, to;
      CCBoard::unmovify(m, from, to);
        out << from << ", " << to << "; " << std::endl;
    } else if (response == "EVAL") {
        out << t_eval.eval(state, state.getPlayer()) << std::endl;
    } else if (tokens[0] == "SAVEZOBRIST") {
        state.SaveZobristTableToFile(tokens[1]);
        out << "Saved Zobrist file to " << tokens[1] << std::endl;
    } else if (tokens[0] == "LOADZOBRIST") {
        state.LoadZobristTableFromFile(tokens[1]);
        out << "Loaded Zobrist file from " << tokens[1] << std::endl;
    } else if (response == "DUMPZOBRIST") {
        out << state.DumpZobrist();
    } else if (response == "GETHASH") {
        out << state.getHash() << std::endl;
    } else if (response == "QUIT") {
        return false;
    } else {
      std::cerr << "Unexpected message " << response << "\n";
    }
    return true;
}

bool Agent::ProcessCommand(std::string response, bool &suggestBreak){
  const std::vector<std::string> tokens = tokenizeMsg(response);
  suggestBreak = false;
  if (tokens.size() == 4 && tokens[0] == "BEGIN" &&
        tokens[1] == "CHINESECHECKERS") {
      // Found BEGIN GAME message, determine if we play first
      current_player = 1;
      if (tokens[2] == name) {
        // We go first!
        opp_name = tokens[3];
        my_player = 1;
        std::cerr << "We're going first!" << std::endl;
        suggestBreak = true;
      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = 2;
        std::cerr << "We're going second!" << std::endl;
        suggestBreak = true;
      } else {
        std::cerr << "Did not find '" << name
                  << "', my name, in the BEGIN command.\n"
                  << "# Found '" << tokens[2] << "' and '" << tokens[3] << "'"
                  << " as player names. Received message '" << response << "'"
                  << std::endl;
        output << "#quit" << std::endl;
        std::exit(EXIT_FAILURE);
      }
    } else if (response == "IDENTIFY") {
      output << "#name " << name << std::endl;
    } else if (response == "DUMPSTATE") {
      output << state.dumpState() << std::endl;
    } else if (tokens[0] == "LOADSTATE") {
      std::string new_state = response.substr(10);
      if (!state.loadState(new_state))
        std::cerr << "Failed to load '" << new_state << "'\n";
    } else if (response == "LISTMOVES") {
      std::vector<uint16_t> moves;
      unsigned from, to;
      state.getMoves(moves);
      for (const auto i : moves){
        CCBoard::unmovify(i, from, to);
        output << from << ", " << to << "; ";
      }
      output << std::endl;
    } else if (tokens[0] == "MOVE") {
      // Just apply the move
      const uint16_t m = state.translateToLocal(tokens);
      if (!state.applyMove(m))
        output << "Unable to apply move " << m << std::endl;
    } else if (response == "GETPLAYER") {
        std::cerr << "Current player is: " << current_player << std::endl;
    } else if (response == "SWAPPLAYER") {
        switchCurrentPlayer();
        std::cerr << "Current player is: " << current_player << std::endl;
    } else if (response == "SWAPSTATEPLAYER") {
        state.swapPlayer();
        std::cerr << "Current state player is: " << state.getPlayer() << std::endl;
    } else if (response == "NEXTMOVE") {
     uint16_t m = nextMove();
     unsigned from, to;
     CCBoard::unmovify(m, from, to);
        output << from << ", " << to << "; " << std::endl;
    } else if (response == "EVAL") {
        output << t_eval.eval(state, state.getPlayer()) << std::endl;
    } else if (tokens[0] == "SAVEZOBRIST") {
        state.SaveZobristTableToFile(tokens[1]);
        output << "Saved Zobrist file to " << tokens[1] << std::endl;
    } else if (tokens[0] == "LOADZOBRIST") {
        state.LoadZobristTableFromFile(tokens[1]);
        output << "Loaded Zobrist file from " << tokens[1] << std::endl;
    } else if (response == "DUMPZOBRIST") {
        output << state.DumpZobrist();
    } else if (response == "GETHASH") {
        output << state.getHash() << std::endl;
    } else if (response == "QUIT") {
        return false;
    } else {
      std::cerr << "Unexpected message " << response << "\n";
    }
    return true;
}

void Agent::trainPlayer()
{
  TDRegression tdr(.05, 0.8);
  std::vector<TDRegression *> p;
  std::vector<uint16_t> moves;
  p.push_back(&tdr);
  p.push_back(&tdr);

  CCBoard s;

  for (int x = 0; x < 10000; x++)
  {
    if (0 == x%100)
      std::cerr << "Playing game " << x << "\n";
    s.reset();

    moves.resize(0);

    while (!s.gameOver())
    {
      double value;
      uint16_t next = p[s.getPlayer()-1]->GetNextAction(s, value, 0.05);
      s.applyMove(next);
      moves.push_back(next);  
    }

    tdr.Train(moves);

  }
  
  // load into td
  t_eval.SetWeights(tdr.GetWeights());


  //Write to a file for faster loading later

  t_eval.WriteWeightsToFile(name + "." + std::to_string(rand() % 50000) + ".regweights");

  // print for ourselves
  fprintf(stderr, "Our weights:\n");
  for (int x = 0; x < 9; x++)
  {
    for (int y = 0; y < 9; y++)
    {
      fprintf(stderr, "%1.3f ", tdr.GetWeights()[x+y*9]);
    }
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "Opponent weights:\n");
  for (int x = 0; x < 9; x++)
  {
    for (int y = 0; y < 9; y++)
    {
      fprintf(stderr, "%1.3f ", tdr.GetWeights()[81+x+y*9]);
    }
    fprintf(stderr, "\n");
  }
}