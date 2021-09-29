#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <iterator>
#include <vector>//necessary?

const int boardPixels = 800;
const int pieceImageSize = 667/2;
const int fieldPixels = boardPixels/8;

class Chessboard : public Player {
  public:
    Chessboard(Chess* gamePointer);
    Chessboard() { };

    void loop_window();
  private:
    void draw_window();
    void make_board();
    void make_pieces();
    void make_pieceSprite(sf::Vector2i pos);

    bool movingPiece(int & movingPieceIndex, sf::Vector2i mousePos, sf::Vector2f & move_with);
    sf::Vector2i origPosPiece(sf::Vector2i mouseToBoard);
    bool ownPiece(sf::Vector2i piecePos);
    bool allowedMove(sf::Vector2i newPiecePos);

    void mseBtnRel(sf::Vector2i & centrePos, const sf::Vector2i & origPos, bool & movePiece, const int & spriteInd);
    void mseBtnPr(bool & movePiece, const sf::Vector2i & mousePos, 
    sf::Vector2i & origPos, sf::Vector2f & move_with, int & movingPieceIndex);

    sf::RenderWindow window;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    sf::Texture pieceTexture;
    std::map <int, int> pieceToImage;
    std::vector <sf::Sprite> pieceSprites;
};//Chessboard

Chessboard::Chessboard(Chess* gamePointer) {
  game = gamePointer;
  pieceTexture.loadFromFile("players/Chessboard/chesspieces.png");
  pieceToImage[W_king] = 1;
  pieceToImage[W_queen] = 2;
  pieceToImage[W_bishop] = 3;
  pieceToImage[W_knight] = 4;
  pieceToImage[W_rook] = 5;
  pieceToImage[W_pawn] = 6;

  pieceToImage[B_king] = -1;
  pieceToImage[B_queen] = -2;
  pieceToImage[B_bishop] = -3;
  pieceToImage[B_knight] = -4;
  pieceToImage[B_rook] = -5;
  pieceToImage[B_pawn] = -6;
  window.create(sf::VideoMode(boardPixels, boardPixels), "Chessboard");
  make_board();
}//Chessboard

void Chessboard::make_board() {
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

void Chessboard::make_pieceSprite(sf::Vector2i pos) {
  int piece = game->board[pos.y][pos.x];		//aanpassen
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

void Chessboard::make_pieces() {
  pieceSprites.clear();
  sf::Vector2i boardPos;
  for(int j = 0; j < boardLength; j++)
    for(int i = 0; i < boardLength; i++) {
      boardPos.x = i; boardPos.y = j;
      make_pieceSprite(boardPos);
    }//for
}//make_pieces

void Chessboard::draw_window() {
  window.draw(boardSprite);
  for(std::vector <sf::Sprite>::iterator it = pieceSprites.begin();
      it != pieceSprites.end(); ++it) {
    window.draw(*it);
  }//for
}//drawWindow

bool Chessboard::movingPiece(int & movingPieceIndex, sf::Vector2i mousePos, sf::Vector2f & move_with) {
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

bool Chessboard::ownPiece(sf::Vector2i piecePos) {
  int x = piecePos.x/fieldPixels;
  int y = piecePos.y/fieldPixels; 
  if(game->whiteTurn && game->board[y][x] > 0)//swap
    return true;
  if(!game->whiteTurn && game->board[y][x] < 0)
    return true;
  return false;
}//ownPiece

sf::Vector2i Chessboard::origPosPiece(sf::Vector2i mouseToBoard) {
  mouseToBoard.x -= mouseToBoard.x %(fieldPixels);
  mouseToBoard.y -= mouseToBoard.y %(fieldPixels);
  return mouseToBoard;
}//originalPositionPiece

bool Chessboard::allowedMove(sf::Vector2i newPiecePos) {
  sf::Vector2i approved(200,200);//remove
  if(newPiecePos == approved)
    return true;
  return false;
}//allowedMove

void Chessboard::mseBtnPr(bool & movePiece, const sf::Vector2i & mousePos, 
sf::Vector2i & origPos, sf::Vector2f & move_with, int & movingPieceIndex) {
  if(!movePiece)
    origPos = origPosPiece(mousePos);
  if(ownPiece(origPos))
    movePiece = movingPiece(movingPieceIndex, mousePos, move_with);
}//mseBtnPr

void Chessboard::mseBtnRel(sf::Vector2i & centrePos, const sf::Vector2i & origPos, 
bool & movePiece, const int & spriteInd) {
  centrePos.x -= (centrePos.x+fieldPixels/2)%fieldPixels - fieldPixels/2;
  centrePos.y -= (centrePos.y+fieldPixels/2)%fieldPixels - fieldPixels/2;
  movePiece = false;
  pieceSprites.at(spriteInd).setPosition(centrePos.x,centrePos.y);
  if(ownPiece(origPos) && !allowedMove(centrePos))
    pieceSprites.at(spriteInd).setPosition(origPos.x,origPos.y);
}//mseBtnRel

void Chessboard::loop_window() {
//window.create(sf::VideoMode(boardPixels, boardPixels), "Chessboard");
  make_pieces();

  bool movePiece = false;
  sf::Vector2f move_with;
  sf::Vector2i origPos;
  sf::Vector2i newPiecePos;
  int movingPieceIndex = 0;
  while(window.isOpen()) {
    sf::Event event;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::MouseButtonPressed:
          if(event.mouseButton.button == sf::Mouse::Left)
            mseBtnPr(movePiece, mousePos, origPos, move_with, movingPieceIndex);
          break;
        case sf::Event::MouseButtonReleased:
          if(event.mouseButton.button == sf::Mouse::Left)
            mseBtnRel(newPiecePos, origPos, movePiece, movingPieceIndex);
          break;
       }//switch
    }//while

    newPiecePos.x = mousePos.x-move_with.x;
    newPiecePos.y = mousePos.y-move_with.y;
    if(movePiece)
        pieceSprites.at(movingPieceIndex).setPosition(newPiecePos.x,newPiecePos.y);

    window.clear(sf::Color::Black);
    draw_window();
    window.display();
  }//while
}//window

