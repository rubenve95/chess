
class Randomplayer : public Player {
  public:
    Randomplayer(Chess* gamePointer);
    void nextMove(Position & oldPos, Position & newPos);

  private:
    Position randomchoice_piece();
    void randomchoice_move(const Position & oldPos, Position & newPos);
};//Randomplayer

Randomplayer::Randomplayer(Chess * gamePointer) {
    game = gamePointer;
}//Randomplayer

void Randomplayer::nextMove(Position & oldPos, Position & newPos) {
  int moveCount = 0;
  while(moveCount == 0) {
    oldPos = randomchoice_piece();
    moveCount = game->compute_moves(oldPos);
  }//while
  randomchoice_move(oldPos, newPos);
}//nextMove

Position Randomplayer::randomchoice_piece() {
  int pieces = game->countPieces();
  if(pieces == 0) {
    std::cout << "WUT" << std::endl;
    game->print();
  }//if
  int randomPiece = rand() % pieces;
  Position pos;
  for(int i = 0; i < boardLength; i++) {
    for(int j = 0; j < boardLength; j++) {
      pos.change_pos(i,j);
      if(game->spotOwnPiece(pos)) {
	randomPiece--;
	if(randomPiece == -1) 
	  return pos;
      }//if
    }//for
  }//for
}//randomchoice_old

void Randomplayer::randomchoice_move(const Position & oldPos, Position & newPos) {
  int nbMoves = game->moves.size();
  int randomMove = rand() % nbMoves;
  std::map<int, Position> :: iterator it;
  for(it = game->moves.begin(); it != game->moves.end(); ++it) {
    if(it->first == randomMove) {
      newPos = it->second;
      break;
    }//if
  }//for
}//randomchoice_new
