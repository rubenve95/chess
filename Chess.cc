#include <iostream>
#include <ctime>         // for time stuff
#include <cstdlib>       // for rand ( )
#include <map>
#include <iterator>

#include "ChessNN.cc"

//std::cout << oldPos.x << " " << oldPos.y << " " << newPos.x << " " << newPos.y << endl;

const int maxTurns = 1000;
const int boardLength = 8;

//black and white
const int whiteStart = boardLength-1;
const int blackStart = 0;
const int whiteMoveForward = -1;
const int blackMoveForward = 1;
const int black = -1;
const int white = 1;

//pieces
const int empty = 0;
const int king = 1;
const int queen = 2;
const int rook = 3;
const int bishop = 4;
const int knight = 5;
const int pawn = 6;

struct Position {
  Position(int x_pos, int y_pos);
  Position();
  Position(const Position & pos);
  Position copyPos(Position pos);
  void change_pos(int x_pos, int y_pos);
  int x, y;
};//Position Struct

Position::Position() {
}//Default Constructor Position

Position::Position(int x_pos, int y_pos) {
  change_pos(x_pos,y_pos);
}//Constructor Position

Position::Position(const Position & pos) {
  change_pos(pos.x,pos.y);
}//Copy Constructor Position

void Position::change_pos(int x_pos, int y_pos) {
  x = x_pos;
  y = y_pos;
}//change_pos

Position Position::copyPos(Position pos) {
  return pos;
}//copyPos

class Player;

class Chess {
  public:

    Chess();

    void initPlayers();
    void initPlayers(ChessNN* whiteNN, ChessNN* blackNN);
    void initPlayers(ChessNN* whiteNN);
    void initPlayers_test(ChessNN* whiteNN);
    int turnsCount;
    int board[boardLength][boardLength];
    bool whiteTurn;
    bool gameOver;
    std::map <int, Position> moves;

    void print();
    bool playGame();
    bool playGame(int & turnsMaxCount);
    bool whiteBestBoard();
    int countPieces();
    bool spotOwnPiece(const Position & newPos);
    bool kingTaken(const int & pieceTaken);
    int compute_moves(const Position & oldPos);
    int domove(const Position & oldPos, const Position & newPos);

  private:

    bool whitePiece(const Position & pos);
    bool blackPiece(const Position & pos);
    void playerPicksMove(Position & oldPos, Position & newPos);
    void pawnToQueen(const Position & pos);

    int kingMove(const Position & oldPos);
    int queenMove(const Position & oldPos);
    int rookMove(const Position & oldPos);
    int bishopMove(const Position & oldPos);
    int knightMove(const Position & oldPos);
    int pawnMove(const Position & oldPos);

    int left_downMove(const Position & oldPos);
    int left_upMove(const Position & oldPos);
    int right_downMove(const Position & oldPos);
    int right_upMove(const Position & oldPos);
    int leftMove(const Position & oldPos);
    int rightMove(const Position & oldPos);
    int upMove(const Position & oldPos);
    int downMove(const Position & oldPos);
    bool insert_move(const Position & oldPos);

    Player* whitePlayer;
    Player* blackPlayer;
};//Chess Class

class Player {
  protected:
    Chess* game;

  public:
    virtual void nextMove(Position & oldPos, Position & newPos) { };
    virtual void printBoard() { };
    virtual double eval_pieces() { };
};//Player Class

#include "players/Randomplayer.cc"
#include "players/AlphaBetaplayer.cc"
#include "players/MonteCarloplayer.cc"
#include "players/Humanplayer.cc"

//Human game vs trained player
void Chess::initPlayers_test(ChessNN* whiteNN) {
  whitePlayer = new AlphaBetaplayer(this, whiteNN);
  blackPlayer = new Humanplayer(this);
}

//Human game vs AlphaBeta
void Chess::initPlayers() {
  whitePlayer = new Humanplayer(this);
  blackPlayer = new AlphaBetaplayer(this);
  //blackPlayer = whitePlayer;
}

//Game between trained players
void Chess::initPlayers(ChessNN* whiteNN, ChessNN* blackNN) {
  whitePlayer = new AlphaBetaplayer(this, whiteNN);
  blackPlayer = new AlphaBetaplayer(this, blackNN);
  //whitePlayer = new Humanplayer(this);
}

//Trained player vs standard AlphaBeta
void Chess::initPlayers(ChessNN* whiteNN) {
  whitePlayer = new AlphaBetaplayer(this, whiteNN);
  blackPlayer = new AlphaBetaplayer(this);
}

Chess::Chess() {
  whiteTurn = true;
  gameOver = false;
  turnsCount = 0;
  moves.clear();

/*
  for(int i = 0; i < boardLength; i++)
    for(int j = 0; j < boardLength; j++)
      board[i][j] = empty;
  board[2][whiteStart] = white*king;
  board[0][blackStart] = black*king;
*/

  for(int i = 0; i < boardLength; i++) {
    board[i][blackStart+blackMoveForward] = black*pawn;
    board[i][whiteStart+whiteMoveForward] = white*pawn;
    for(int j = 2; j < boardLength - 2; j++)
      board[i][j] = empty;
  }//for

  board[0][whiteStart] = white*rook;
  board[1][whiteStart] = white*knight;
  board[2][whiteStart] = white*bishop;
  board[3][whiteStart] = white*queen;
  board[4][whiteStart] = white*king;
  board[5][whiteStart] = white*bishop;
  board[6][whiteStart] = white*knight;
  board[7][whiteStart] = white*rook;

  board[0][blackStart] = black*rook;
  board[1][blackStart] = black*knight;
  board[2][blackStart] = black*bishop;
  board[3][blackStart] = black*queen;
  board[4][blackStart] = black*king;
  board[5][blackStart] = black*bishop;
  board[6][blackStart] = black*knight;
  board[7][blackStart] = black*rook;
}//Chess

void Chess::print() {
  std::cout << std::endl;
  if(whiteTurn)
    std::cout << "White ";
  else
    std::cout << "Black ";
  if(gameOver)
    std::cout << "has lost the game!" << std::endl << std::endl;
  else
    std::cout << "turn: " << turnsCount << std::endl << std::endl;
  for(int j = boardLength-1; j >= 0; j--) {
    for(int i = 0; i < boardLength; i++) {
      if(board[i][j] >= 0)
	std::cout << " ";
      std::cout << board[i][j] << " ";
    }//for
    std::cout << std::endl;
  }//for
}//print

void Chess::playerPicksMove(Position & oldPos, Position & newPos) {
  if(whiteTurn) {
    blackPlayer->printBoard();
    whitePlayer->nextMove(oldPos,newPos);
  }//if
  else {
    whitePlayer->printBoard();
    blackPlayer->nextMove(oldPos,newPos);
  }//else
}//playerPicksMove

//does a move and returns the value of the board position taken
int Chess::domove(const Position & oldPos, const Position & newPos) {
  int moveFrom_value = board[oldPos.x][oldPos.y];
  int moveTo_value = board[newPos.x][newPos.y];
  if(!spotOwnPiece(oldPos))
    return king;
  board[newPos.x][newPos.y] = moveFrom_value;
  board[oldPos.x][oldPos.y] = empty;
  pawnToQueen(newPos);
  whiteTurn = !whiteTurn;
  return moveTo_value;
}//domove

bool Chess::kingTaken(const int & pieceTaken) {
  if(pieceTaken == white*king || pieceTaken == black*king)
    return true;
  return false;
}//kingTaken

void Chess::pawnToQueen(const Position & pos) {
  if(board[pos.x][pos.y] == black*pawn && pos.y == whiteStart)
    board[pos.x][pos.y] = black*queen;
  else if(board[pos.x][pos.y] == white*pawn && pos.y == blackStart)
    board[pos.x][pos.y] = white*queen;
}//pawnToQueen

int Chess::kingMove(const Position & oldPos) {
  int x_lower = -1, x_upper = 1, y_lower = -1, y_upper = 1;
  int movesCount = 0;
  if(oldPos.x == 0)
    x_lower = 0;
  else if(oldPos.x == boardLength-1)
    x_upper = 0;
  if(oldPos.y == 0)
    y_lower = 0;
  else if(oldPos.y == boardLength-1)
    y_upper = 0;  
  Position pos;
  for(int x = oldPos.x+x_lower; x <= oldPos.x+x_upper; x++)
    for(int y = oldPos.y+y_lower; y <= oldPos.y+y_upper; y++) {
      pos.change_pos(x,y);
      if(insert_move(pos))
	movesCount++;
    }//for
  return movesCount;
}//kingMove

int Chess::queenMove(const Position & oldPos) {
  int movesCount = 0;
  movesCount += rightMove(oldPos);
  movesCount += leftMove(oldPos);
  movesCount += upMove(oldPos);
  movesCount += downMove(oldPos);

  movesCount += right_upMove(oldPos);
  movesCount += right_downMove(oldPos);
  movesCount += left_upMove(oldPos);
  movesCount += left_downMove(oldPos);
  return movesCount;
}//queenMove

int Chess::rookMove(const Position & oldPos) {
  int movesCount = 0;
  movesCount += rightMove(oldPos);
  movesCount += leftMove(oldPos);
  movesCount += upMove(oldPos);
  movesCount += downMove(oldPos);
  return movesCount;
}//rookMove

int Chess::bishopMove(const Position & oldPos) {
  int movesCount = 0;
  movesCount += right_upMove(oldPos);
  movesCount += right_downMove(oldPos);
  movesCount += left_upMove(oldPos);
  movesCount += left_downMove(oldPos);
  return movesCount;
}//bishopMove

int Chess::knightMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos;
  for(int x = oldPos.x - 2; x <= oldPos.x + 2; x++) {
    if(x == oldPos.x || x < 0 || x > boardLength-1)
      continue;
    for(int y = oldPos.y - 2; y <= oldPos.y + 2; y++) {
      if(y == oldPos.y || abs(oldPos.x - x) == abs(oldPos.y - y) || y < 0 || y > boardLength-1)
	continue;
      pos.change_pos(x,y);
      if(insert_move(pos))
        movesCount++;
    }//for
  }//for
  return movesCount;
}//knightMove

int Chess::pawnMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  if(whiteTurn) {
    pos.change_pos(oldPos.x,oldPos.y+whiteMoveForward);
    if(board[pos.x][pos.y] == empty)
      if(insert_move(pos)) {
        movesCount++;
        if(oldPos.y == whiteStart+whiteMoveForward) {
          pos.change_pos(pos.x,pos.y+whiteMoveForward);
	  if(board[pos.x][pos.y] == empty)
            if(insert_move(pos))
	      movesCount++;
        }//if
      }//if
    pos.change_pos(oldPos.x-1,oldPos.y+whiteMoveForward);
    if(oldPos.x > 0 && blackPiece(pos))
      if(insert_move(pos))
	movesCount++;
    pos.change_pos(oldPos.x+1,oldPos.y+whiteMoveForward);
    if(oldPos.x < boardLength-1 && blackPiece(pos))
      if(insert_move(pos))
	movesCount++;
  }//if
  else {
    pos.change_pos(oldPos.x,oldPos.y+blackMoveForward);
    if(board[pos.x][pos.y] == empty)
      if(insert_move(pos)) {
        movesCount++;
        if(oldPos.y == blackStart+blackMoveForward) {
          pos.change_pos(pos.x,pos.y+blackMoveForward);
	  if(board[pos.x][pos.y] == empty)
            if(insert_move(pos))
	      movesCount++;
        }//if
      }//if
    pos.change_pos(oldPos.x-1,oldPos.y+blackMoveForward);
    if(oldPos.x > 0 && whitePiece(pos))
      if(insert_move(pos))
	movesCount++;
    pos.change_pos(oldPos.x+1,oldPos.y+blackMoveForward);
    if(oldPos.x < boardLength-1 && whitePiece(pos))
      if(insert_move(pos))
	movesCount++;
  }//else
  return movesCount;
}//pawnMove

int Chess::rightMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != boardLength-1) {
    pos.change_pos(pos.x+1,pos.y);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//rightMove

int Chess::leftMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != 0) {
    pos.change_pos(pos.x-1,pos.y);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//leftMove

int Chess::upMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.y != boardLength-1) {
    pos.change_pos(pos.x,pos.y+1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//upMove

int Chess::downMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.y != 0) {
    pos.change_pos(pos.x,pos.y-1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//downMove

int Chess::right_upMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != boardLength-1 && pos.y != boardLength-1) {
    pos.change_pos(pos.x+1,pos.y+1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//right_upMove

int Chess::right_downMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != boardLength-1 && pos.y != 0) {
    pos.change_pos(pos.x+1,pos.y-1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//right_downMove

int Chess::left_upMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != 0 && pos.y != boardLength-1) {
    pos.change_pos(pos.x-1,pos.y+1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//left_upMove

int Chess::left_downMove(const Position & oldPos) {
  int movesCount = 0;
  Position pos(oldPos);
  while(pos.x != 0 && pos.y != 0) {
    pos.change_pos(pos.x-1,pos.y-1);
    if(insert_move(pos)) {
      movesCount++;
    }//if
    if(board[pos.x][pos.y] != empty)
      break;
  }//while
  return movesCount;
}//left_downMove

bool Chess::insert_move(const Position & newPos) {
  int nbMoves;
  if(!spotOwnPiece(newPos)) {
    nbMoves = moves.size();
    moves[nbMoves] = newPos;
    return true;
  }//if
  return false;
}//insert_move

bool Chess::spotOwnPiece(const Position & Pos) {
  if(whitePiece(Pos) && whiteTurn)
    return true;
  if(blackPiece(Pos) && !whiteTurn)
    return true;
  return false;
}//spotTaken

bool Chess::whitePiece(const Position & pos) {
  if(white*board[pos.x][pos.y] > 0)
    return true;
  return false;
}//whitePiece

bool Chess::blackPiece(const Position & pos) {
  if(black*board[pos.x][pos.y] > 0)
    return true;
  return false;
}//blackPiece

int Chess::compute_moves(const Position & oldPos) {
  int movesCount = 0;
  moves.clear();
  switch(board[oldPos.x][oldPos.y]) {
    case white*king: case black*king:
      movesCount = kingMove(oldPos);
      break;
    case white*queen: case black*queen:
      movesCount = queenMove(oldPos);
      break;
    case white*rook: case black*rook:
      movesCount = rookMove(oldPos);
      break;
    case white*bishop: case black*bishop:
      movesCount = bishopMove(oldPos);
      break;
    case white*knight: case black*knight:
      movesCount = knightMove(oldPos);
      break;
    case white*pawn: case black*pawn:
      movesCount = pawnMove(oldPos);
      break;
    default:
      break;
  }//switch
  return movesCount;
}//compute_moves

int Chess::countPieces() {
  int pieces = 0;
  Position pos;
  for(int i = 0; i < boardLength; i++)
    for(int j = 0; j < boardLength; j++) {
      pos.change_pos(i,j);
      if(spotOwnPiece(pos))
	pieces++;
    }//for
  return pieces;
}//countPieces

bool Chess::whiteBestBoard() {
  int whiteBoard = white*whitePlayer->eval_pieces();
  int blackBoard = black*blackPlayer->eval_pieces();
  return ((whiteBoard > blackBoard) ? true : false);
}//evalBoard

bool Chess::playGame() {
  int pieceTaken = 0;
  Position oldPos(0,0), newPos(0,0);
  while(!gameOver) {
    //print();
    playerPicksMove(oldPos, newPos);
    pieceTaken = domove(oldPos, newPos);
    gameOver = kingTaken(pieceTaken);
  }//while
  //print();
  return !whiteTurn;
}//playGame

bool Chess::playGame(int & turnsMaxCount) {
  int pieceTaken = 0;
  Position oldPos(0,0), newPos(0,0);
  while(!gameOver) {
    playerPicksMove(oldPos, newPos);
    pieceTaken = domove(oldPos, newPos);
    gameOver = kingTaken(pieceTaken);
    turnsCount++;
    if(turnsCount == maxTurns) {
      turnsMaxCount++;
      return whiteBestBoard();
    }//if
  }//while
  //print();
  return !whiteTurn;
}//playGame

