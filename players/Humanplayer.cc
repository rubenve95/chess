#include <SFML/Graphics.hpp>
#include <map>
#include <iterator>
#include <vector>

const int boardPixels = 800;
const int pieceImageSize = 667/2;
const int fieldPixels = boardPixels/8;

class Humanplayer : public Player {
  public:
    Humanplayer(Chess* gamePointer);
    void nextMove(Position & oldPos, Position & newPos);
    void printBoard();
    Humanplayer() { };

  private:
    void loop_window(Position & oldPos, Position & newPos);
    void draw_window();
    void make_board();
    void make_pieces();
    void make_pieceSprite(const sf::Vector2i & pos);
    void init_piecesImage();

    bool movingPiece(int & movingPieceIndex, sf::Vector2i mousePos, sf::Vector2f & move_with);
    sf::Vector2i origPosPiece(sf::Vector2i mouseToBoard);
    bool ownPiece(const sf::Vector2i & piecePos);
    bool allowedMove(const Position & newPos);
    void oppMoveAnimation(const Position & oldPos, const Position & newPos);
    int spriteAtLocation(const sf::Vector2i & loc);

    bool mseBtnRel(sf::Vector2i & centrePos, const sf::Vector2i & origPos,
      bool & movePiece, const int & spriteInd, Position & newPos);
    void mseBtnPr(bool & movePiece, const sf::Vector2i & mousePos, 
      sf::Vector2i & origPos, sf::Vector2f & move_with, int & movingPieceIndex, Position & newPos);

    sf::RenderWindow window;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    sf::Texture pieceTexture;
    std::map <int, int> pieceToImage;
    std::vector <sf::Sprite> pieceSprites;
};//Humanplayer

Humanplayer::Humanplayer(Chess* gamePointer) {
  game = gamePointer;
  init_piecesImage();
  window.create(sf::VideoMode(boardPixels, boardPixels), "Chessboard");
  make_board();
}//Humanplayer

void Humanplayer::init_piecesImage() {
  pieceTexture.loadFromFile("players/Chessboard/chesspieces.png");
  pieceToImage[white*king] = 1;
  pieceToImage[white*queen] = 2;
  pieceToImage[white*bishop] = 3;
  pieceToImage[white*knight] = 4;
  pieceToImage[white*rook] = 5;
  pieceToImage[white*pawn] = 6;
  pieceToImage[black*king] = -1;
  pieceToImage[black*queen] = -2;
  pieceToImage[black*bishop] = -3;
  pieceToImage[black*knight] = -4;
  pieceToImage[black*rook] = -5;
  pieceToImage[black*pawn] = -6;
}//init_piecesImage

void Humanplayer::nextMove(Position & oldPos, Position & newPos) {
  loop_window(oldPos, newPos);
}//nextMove

void Humanplayer::printBoard() {
  if(window.isOpen()) {
    make_pieces();
    draw_window();
  }//if
}//printBoard

void Humanplayer::make_board() {
  sf::Image boardImage;
  boardImage.create(boardPixels, boardPixels, sf::Color::Black);
  bool isBlackPixel = false;
  for(int i = 0; i < boardPixels; i++) {
    for(int j = 0; j < boardPixels; j++) {
      if(isBlackPixel)
        boardImage.setPixel(i, j, sf::Color(0,100,0)); //dark green
      else
        boardImage.setPixel(i, j, sf::Color::White);
      if(!(j%fieldPixels))
	isBlackPixel = !isBlackPixel;
    }//for
    if(!(i%fieldPixels))
      isBlackPixel = !isBlackPixel;
  }//for
  boardTexture.loadFromImage(boardImage);
  boardSprite.setTexture(boardTexture);
}//make_board

void Humanplayer::make_pieceSprite(const sf::Vector2i & pos) {
  int piece = game->board[pos.x][pos.y];
  if(piece != empty) {
    int x_image = ( abs(pieceToImage[piece]) - 1) * pieceImageSize;
    int y_image = 0;
    if(pieceToImage[piece] < 0)
      y_image = pieceImageSize;

    sf::Sprite sprite;
    sprite.setTexture(pieceTexture);
    sprite.setTextureRect(sf::IntRect(x_image,y_image,pieceImageSize,pieceImageSize));

    float rescale = (float)fieldPixels/pieceImageSize;
    sprite.setScale(rescale, rescale);
    sprite.setPosition(pos.x*fieldPixels,pos.y*fieldPixels);
    pieceSprites.push_back(sprite);  
  }//if
}//make_pieceSprite

void Humanplayer::make_pieces() {
  pieceSprites.clear();
  sf::Vector2i boardPos;
  for(int j = 0; j < boardLength; j++)
    for(int i = 0; i < boardLength; i++) {
      boardPos.x = i; boardPos.y = j;
      make_pieceSprite(boardPos);
    }//for
}//make_pieces

void Humanplayer::draw_window() {
  window.draw(boardSprite);
  for(std::vector <sf::Sprite>::iterator it = pieceSprites.begin();
      it != pieceSprites.end(); ++it) {
    window.draw(*it);
  }//for
  window.display();
}//drawWindow

bool Humanplayer::movingPiece(int & movingPieceIndex, sf::Vector2i mousePos, sf::Vector2f & move_with) {
  int notfoundIndex = movingPieceIndex;
  movingPieceIndex = 0;
  move_with.x = 0; move_with.y = 0;
  for(std::vector <sf::Sprite>::iterator it = pieceSprites.begin();
      it != pieceSprites.end(); ++it) {
    if( (*it).getGlobalBounds().contains( mousePos.x, mousePos.y)) {
      move_with.x = mousePos.x - (*it).getPosition().x;
      move_with.y = mousePos.y - (*it).getPosition().y;
      return true;
    }//if
    movingPieceIndex++;
  }//for
  movingPieceIndex = notfoundIndex;
  return false;
}//movingPiece

bool Humanplayer::ownPiece(const sf::Vector2i & piecePos) {
  Position pos(piecePos.x/fieldPixels, piecePos.y/fieldPixels);
  if(game->spotOwnPiece(pos))
    return true;
  return false;
}//ownPiece

sf::Vector2i Humanplayer::origPosPiece(sf::Vector2i mouseToBoard) {
  mouseToBoard.x -= mouseToBoard.x %(fieldPixels);
  mouseToBoard.y -= mouseToBoard.y %(fieldPixels);
  return mouseToBoard;
}//originalPositionPiece

bool Humanplayer::allowedMove(const Position & newPos) {
  if(game->moves.empty())
    return false;
  for(std::map <int, Position>::iterator it = game->moves.begin();
      it != game->moves.end(); ++it)
    if((it->second).x == newPos.x && (it->second).y == newPos.y)
      return true;
  return false;
}//allowedMove

void Humanplayer::mseBtnPr(bool & movePiece, const sf::Vector2i & mousePos, 
sf::Vector2i & origPos, sf::Vector2f & move_with, int & movingPieceIndex, Position & oldPos) {
  if(!movePiece) {
    origPos = origPosPiece(mousePos);
    oldPos.change_pos(origPos.x/fieldPixels,origPos.y/fieldPixels);
  }//if
  if(game->compute_moves(oldPos) > 0 && ownPiece(origPos))
    movePiece = movingPiece(movingPieceIndex, mousePos, move_with);
}//mseBtnPr

//Sets the piece at the correct position
//returns false if it must be set back to the original position
bool Humanplayer::mseBtnRel(sf::Vector2i & centrePos, const sf::Vector2i & origPos, 
bool & movePiece, const int & spriteInd, Position & newPos) {
  movePiece = false;
  centrePos.x -= (centrePos.x+fieldPixels/2)%fieldPixels - fieldPixels/2;
  centrePos.y -= (centrePos.y+fieldPixels/2)%fieldPixels - fieldPixels/2;
  newPos.change_pos(centrePos.x/fieldPixels, centrePos.y/fieldPixels);
  if(ownPiece(origPos) && !allowedMove(newPos)) {
    pieceSprites.at(spriteInd).setPosition(origPos.x,origPos.y);
    return false;
  }//if
  if(allowedMove(newPos)) {
    pieceSprites.at(spriteInd).setPosition(centrePos.x,centrePos.y);
    return true;
  }//if
  return false;
}//mseBtnRel

int Humanplayer::spriteAtLocation(const sf::Vector2i & loc) {
  int movingPieceIndex = 0;
  sf::Vector2f locF(loc.x,loc.y);
  for(std::vector <sf::Sprite>::iterator it = pieceSprites.begin();
    it != pieceSprites.end(); ++it) {
    if( (*it).getPosition() == locF)
      return movingPieceIndex;
    movingPieceIndex++;
  }//for
  return -1;
}//spriteAtLocation

void Humanplayer::oppMoveAnimation(const Position & oldPos, const Position & newPos) {
  sf::Vector2i loc(oldPos.x*fieldPixels,oldPos.y*fieldPixels);
  int movingPieceIndex = spriteAtLocation(loc);
  if(game->board[oldPos.x][oldPos.y] == empty && movingPieceIndex != -1) {
    int totalmsec = 1000;
    sf::Vector2i speed(loc.x/totalmsec,loc.y/totalmsec);
    for(int i = 0; i < totalmsec; i++) {
      loc.x += speed.x; loc.y += speed.y;
      pieceSprites.at(movingPieceIndex).setPosition(loc.x, loc.y);
      sf::sleep(sf::milliseconds(1));
      draw_window();
    }//for
  }//if
}//oppMoveAnimation

void Humanplayer::loop_window(Position & oldPos, Position & newPos) {
  oppMoveAnimation(oldPos,newPos);
  bool movePiece = false, moveDone = false;
  sf::Vector2f move_with;
  sf::Vector2i newPiecePos, origPos;
  int movingPieceIndex = 0;
  make_pieces();
  while(window.isOpen() && !game->gameOver) {
    make_pieces();
    sf::Event event;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          game->gameOver = true;
          break;
        case sf::Event::MouseButtonPressed:
          if(event.mouseButton.button == sf::Mouse::Left)
            mseBtnPr(movePiece, mousePos, origPos, move_with, movingPieceIndex, oldPos);
          break;
        case sf::Event::MouseButtonReleased:
          if(event.mouseButton.button == sf::Mouse::Left)
            moveDone = mseBtnRel(newPiecePos, origPos, movePiece, movingPieceIndex, newPos);
          break;
       }//switch
    }//while

    newPiecePos.x = mousePos.x-move_with.x;
    newPiecePos.y = mousePos.y-move_with.y;
    if(movePiece)
      pieceSprites.at(movingPieceIndex).setPosition(newPiecePos.x,newPiecePos.y);

    draw_window();
    if(moveDone)
      break;
  }//while
}//window

