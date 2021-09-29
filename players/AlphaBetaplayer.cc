//values
const int val_queen = 9;
const int val_rook = 5;
const int val_bishop = 3;
const int val_knight = 3;
const int val_pawn = 1;
const int val_king = 100;
const int val_MAX = 1000;

class AlphaBetaplayer : public Player {
public:
    AlphaBetaplayer(Chess* gamePointer);
    AlphaBetaplayer(Chess* gamePointer, ChessNN* NeuralNet);

    void nextMove(Position & oldPos, Position & newPos);
    double eval_pieces();
    double eval_NN();
private:
    int pieceToValue(int piece);
    int giveColor();
    bool firstTurn(const int & qui_depth);
    double AlphaBeta(int depth, int qui_depth, double parentValue, Position & bestOldPos, Position & bestNewPos);
    int AlphaBetaDepth;
    int quiescenceDepth;
    double (AlphaBetaplayer::*evalPointer)();
    ChessNN* NN;
};//AlphaBetaplayer Class

AlphaBetaplayer::AlphaBetaplayer(Chess * gamePointer) {
  game = gamePointer;
  AlphaBetaDepth = 2;
  quiescenceDepth = AlphaBetaDepth + 2;
  evalPointer = &AlphaBetaplayer::eval_pieces;
}//AlphaBetaplayer Constr

AlphaBetaplayer::AlphaBetaplayer(Chess * gamePointer, ChessNN* NeuralNet) {
  game = gamePointer;
  AlphaBetaDepth = 2;
  quiescenceDepth = AlphaBetaDepth + 2;
  NN = NeuralNet;
  evalPointer = &AlphaBetaplayer::eval_NN;
}//AlphaBetaplayer Constr

void AlphaBetaplayer::nextMove(Position & oldPos, Position & newPos) {
  AlphaBeta(AlphaBetaDepth, quiescenceDepth, -val_MAX, oldPos, newPos);
}//nextMove

int AlphaBetaplayer::giveColor() {
  if(game->whiteTurn)
    return white;
  else
    return black;
}//giveColor

int AlphaBetaplayer::pieceToValue(int piece) {
  int color;
  if(white*piece > 0)
    color = white;
  else
    color = black;
  switch(color*piece) {
    case empty:
      return 0;
    case king:
      return color*val_king;
    case queen:
      return color*val_queen;
    case rook:
      return color*val_rook;
    case bishop:
      return color*val_bishop;
    case knight:
      return color*val_knight;
    case pawn:
      return color*val_pawn;
  }//switch
}//pieceToValue

bool AlphaBetaplayer::firstTurn(const int & qui_depth) {
  if(qui_depth == quiescenceDepth)
    return true;
  return false;
}//firstTurn

double AlphaBetaplayer::eval_NN() {
  double output = 200*NN->NeuralNet(game->board, giveColor() );
  return giveColor()*output + eval_pieces();
}//eval_NN

double AlphaBetaplayer::eval_pieces() {
  double total = 0;
  for(int i = 0; i < boardLength; i++) 
    for(int j = 0; j < boardLength; j++)
      total += pieceToValue(game->board[i][j]);
  return total;
}//eval_pieces

double AlphaBetaplayer::AlphaBeta(int depth, int qui_depth, double parentValue, Position & bestOldPos, Position & bestNewPos) {
  if(depth == 0 || qui_depth == 0)
    return (this->*evalPointer)();
  int pieceTaken = 0;
  double childValue;
  double bestValue = -val_MAX, pruningValue = bestValue+1;
  int color = giveColor();
  Position oldPos, newPos;
  Chess copy = *game;
  for(int i = 0; i < boardLength; i++)
    for(int j = 0; j < boardLength; j++) {
      oldPos.change_pos(i,j);
      if(game->spotOwnPiece(oldPos))
        if(game->compute_moves(oldPos) > 0) {
          copy = *game;
          for(std::map<int, Position> :: iterator it = copy.moves.begin(); it != copy.moves.end(); ++it) {
            newPos = it->second;
            pieceTaken = game->domove(oldPos, newPos);
            if(game->kingTaken(pieceTaken)) {
              *game = copy;
              if(firstTurn(qui_depth)) {
                bestOldPos = oldPos;
                bestNewPos = newPos;
              }//if
              return color*val_king;
            }//if
            else if(pieceTaken == empty)
              childValue = color*AlphaBeta(depth-1,qui_depth-1,bestValue,bestOldPos,bestNewPos);
            else
              childValue = color*AlphaBeta(depth,qui_depth-1,bestValue,bestOldPos,bestNewPos);

            if(childValue > bestValue) {
              bestValue = childValue;
              if(firstTurn(qui_depth)) {
                bestOldPos = oldPos;
                bestNewPos = newPos;
              }//if
              else
                pruningValue = -bestValue;
            }//if
            *game = copy;
            if(pruningValue <= parentValue && !firstTurn(qui_depth))
              return color*bestValue;
          }//for
        }//if
    }//for
   return color*bestValue;
}//AlphaBeta
