
class MonteCarloplayer : public Player {
  public:
    MonteCarloplayer(Chess* gamePointer);
    void nextMove(Position & oldPos, Position & newPos);
  private:
    void copy_board(int (&copy)[boardLength][boardLength], const int (&orig)[boardLength][boardLength]);
    int playRandomGames(const bool & turnCopy);
    int maxGames;
};//MonteCarloplayer

MonteCarloplayer::MonteCarloplayer(Chess * gamePointer) {
  game = gamePointer;
  maxGames = 10000;
}//MonteCarloplayer

void MonteCarloplayer::nextMove(Position & bestOld, Position & bestNew) {
  int boardCopy[boardLength][boardLength];
  copy_board(boardCopy, game->board);
  bool turnCopy = game->whiteTurn;
  int bestValue = -maxGames, gamesValue;
  int pieceTaken;
  Position oldPos, newPos;
  for(int i = 0; i < boardLength; i++)
    for(int j = 0; j < boardLength; j++) {
      oldPos.change_pos(i,j);
      if(game->spotOwnPiece(oldPos))
        if(game->compute_moves(oldPos) > 0) {
          for(std::map<int, Position> :: iterator it = game->moves.begin(); it != game->moves.end(); ++it) {
            newPos = it->second;
            pieceTaken = game->domove(oldPos, newPos);
            if(game->kingTaken(pieceTaken)) {
              bestOld = oldPos;
              bestNew = newPos;
              copy_board(game->board,boardCopy);
              game->whiteTurn = turnCopy;
              return;
            }//if
            else
              gamesValue = playRandomGames(turnCopy);
            if(gamesValue > bestValue) {
              bestValue = gamesValue;
              bestOld = oldPos;
              bestNew = newPos;
            }//if
            copy_board(game->board,boardCopy);
            game->whiteTurn = turnCopy;
          }//for
        }//if
    }//for
}//nextMove

void MonteCarloplayer::copy_board(int (&copy)[boardLength][boardLength], const int (&orig)[boardLength][boardLength]) {
  for(int i = 0; i < boardLength; i++)
    for(int j = 0; j < boardLength; j++)
      copy[i][j] = orig[i][j];
}//copyBoard

int MonteCarloplayer::playRandomGames(const bool & turnCopy) {
  int winner = 0;
  int pieceTaken;
  for(int i = 0; i < maxGames; i++) {
    Chess copy = *game;
    Player* randomPlayer = new Randomplayer(&copy);
    Position oldPos, newPos;
    while(!copy.gameOver) {
      randomPlayer->nextMove(oldPos,newPos);
      pieceTaken = copy.domove(oldPos,newPos);
      copy.gameOver = copy.kingTaken(pieceTaken);
    }//while
    if( (white*pieceTaken > 0 && !turnCopy) || (black*pieceTaken > 0 && turnCopy) )
      winner++;
    else
      winner--;
  }//for
  return winner;
}//playRandomGames
