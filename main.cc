#include "GeneticAlg.cc"

int main(int argc, char* argv[ ]) {

  //for training:

  if ( argc != 2 ) {
    std::cout << "Use: " << argv[0] << " <epochs>" << std::endl;
    return 1;  
  }//if
  int epochs = atoi (argv[1]);
  GeneticAlg genChess(epochs);
  genChess.train();
  //genChess.testGame(epochs);

  //for playing a game:

  //Chess chess;
  //chess.initPlayers();
  //chess.playGame();
  return 0;
}//main
