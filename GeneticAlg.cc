#include <fstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <ctime>         // for time stuff
#include <cstdlib>       // for rand ( )

#include "Chess.cc"

const int MaxPop = 100;

class GeneticAlg {
  public:
    GeneticAlg() { };
    GeneticAlg(int ep);
    void train();

    void testGame(const int & opponent);

  private:
    int population;
    int epochs;
    int parentsPerChild;
    double mutation_add;
    double mutationRate;
    bool allTimeWinners[MaxPop];
    ChessNN* NN[MaxPop];

    void firstInit();

    void printWinLose(const int (&winners)[MaxPop/2], const int (&losers)[MaxPop/2]);
    void NNfromFile();
    void saveNN();
    void setOpponents(int (&opponent)[MaxPop]);
    void insertWinners(int (&winners)[MaxPop/2], int (&losers)[MaxPop/2], 
         const int & playerOne, const int & playerTwo, const bool & playerOneWins, const int & gamesPlayed);
    void getRandomOrder(int (&winners)[MaxPop/2]);
    void make_children(const int (&winners)[MaxPop/2], const int (&losers)[MaxPop/2]);
    void make_child(const int (&parentsIndices)[MaxPop/2], const int & childIndex);
    void mutate();
    bool NNwins();
    void setAllTimeWinners(const int (&losers)[MaxPop/2]);
    void results(const int & maxTurnsCount, const int & ep, int & testWins);
};

GeneticAlg::GeneticAlg(int ep) {
  srand (time(NULL));
  epochs = ep;
  population = 30;
  parentsPerChild = 2;
  mutation_add = 0.1;
  mutationRate = 0.1;
  for(int i = 0; i < population; i++) {
    NN[i] = new ChessNN();
    allTimeWinners[i] = true;
  }//for
  //firstInit();
}//geneticAlg

void GeneticAlg::firstInit() {
  for(int i = 0; i < population; i++)
    NN[i]->initWeights();
  saveNN();
}

void GeneticAlg::testGame(const int & opponent) {
  if(opponent >= population)
    std::cout << "pick a valid member of the population." << std::endl;
  NNfromFile();
  Chess chessgame;
  chessgame.initPlayers_test(NN[opponent]);
  chessgame.playGame();
}

void GeneticAlg::train() {
  int winners[MaxPop/2];
  int losers[MaxPop/2];
  int opponent[MaxPop];
  int playerOne, playerTwo;
  bool playerOneWins;
  int maxTurnsCount = 0;
  int testWins = 0;
  int gamesPlayed;
  NNfromFile();
  for(int i = 0; i < epochs; i++) {
    setOpponents(opponent);
    gamesPlayed = 0;
    for(int j = 0; j < population; j++)
      if(opponent[j] > j) {
        Chess chessgame;
        playerOne = (rand() % 2) ? j : opponent[j];
        playerTwo = opponent[playerOne];
        chessgame.initPlayers(NN[playerOne], NN[playerTwo]);
        playerOneWins = chessgame.playGame(maxTurnsCount);
        insertWinners(winners, losers, playerOne, playerTwo, playerOneWins, gamesPlayed);
        gamesPlayed++;
      }//if
    setAllTimeWinners(losers);
    make_children(winners, losers);
    mutate();
    saveNN();
    results(maxTurnsCount, i, testWins);
  }//for
}//train

void GeneticAlg::setAllTimeWinners(const int (&losers)[MaxPop/2]) {
  for(int i = 0; i < population/2; i++)
    if(allTimeWinners[ losers[i] ])
      allTimeWinners[ losers[i] ] = false;
}//for

void GeneticAlg::setOpponents(int (&opponent)[MaxPop]) {
  bool hasOpp[population];
  int opp;
  for(int i = 0; i < population; i++)
    hasOpp[i] = false;
  for(int i = 0; i < population; i++)
    if(!hasOpp[i]) {
      opp = rand () % population;
      while( (opp == i || hasOpp[opp]) && opp < population) {
        opp++;
        if(opp == population)
          opp = 0;
      }
      hasOpp[i] = true;
      hasOpp[opp] = true;
      opponent[i] = opp;
      opponent[opp] = i;
    }//if
}//setOpponents

void GeneticAlg::results(const int & maxTurnsCount, const int & ep, int & testWins) {
  std::cout << "Trained for " << ep+1 << " epochs, " << maxTurnsCount;
  std::cout << " out of " << population*(ep+1)/2 << " reached max turns (";
  std::cout << (100*2*maxTurnsCount)/(population*(ep+1)) << "%)";
  if(NNwins())
    testWins++;
  std::cout << ", won against standard AlphaBeta " << testWins << " times";
  std::cout << " (" << (100*testWins)/(ep+1) << "%)";

  std::cout << " Champions:";
  bool noWinners = true;
  for(int i = 0; i < population; i++)
    if(allTimeWinners[i]) {
      std::cout << " " << i;
      noWinners = false;
    }//if
  if(noWinners)
    std::cout << " None";
  std::cout << std::endl;
  /*
  else
    std::cout << "A chosen NN LOSES against a MonteCarlo player" << std::endl;
  if(ep != 0 && ep != epochs-1) {
    if( (100*2*(maxTurnsCount-prevTurns))/population == (100*2*prevTurns)/(population*ep) && maxMutMult < 3) {
      mutationRate *= 1.5;
      maxMutMult *= 1.2;
      std::cout << "Mutationrate and the Maxmutation were increased to ";
      std::cout << mutationRate << " " << maxMutMult << std::endl;
    }//if
    else if(maxMutMult > 2){
      mutationRate /= 1.5;
      maxMutMult /= 1.2;
    }//else
  }//if
  prevTurns = maxTurnsCount;
*/ 
}//results

bool GeneticAlg::NNwins() {
  int doesNothing = 0;
  Chess chess;
  chess.initPlayers(NN[0]);
  return chess.playGame(doesNothing);
}//NNwins

void GeneticAlg::insertWinners(int (&winners)[MaxPop/2], int (&losers)[MaxPop/2], 
  const int & playerOne, const int & playerTwo, const bool & playerOneWins, const int & gamesPlayed) {
  if(playerOneWins) {
    winners[gamesPlayed] = playerOne;
    losers[gamesPlayed] = playerTwo;
    }//if
  else {
    winners[gamesPlayed] = playerTwo;
    losers[gamesPlayed] = playerOne;
  }//else
}//insertWinners

void GeneticAlg::getRandomOrder(int (&winners)[MaxPop/2]) {
  int randomNb;
  int temp;
  for(int i = 0; i < population/2; i++) {
    randomNb = rand() % (population/2);
    temp = winners[randomNb];
    winners[randomNb] = winners[i];
    winners[i] = temp;
  }//for
}//getRandomOrder

void GeneticAlg::printWinLose(const int (&winners)[MaxPop/2], const int (&losers)[MaxPop/2]) {
  std::cout << "Winners:";
  for(int i = 0; i < population/2; i++) {
    std::cout << " " << winners[i];
    //std::cout << "loser: " << losers[i] << std::endl;
  }//for
  std::cout << " ";
}//printWinLose


void GeneticAlg::make_children(const int (&winners)[MaxPop/2], const int (&losers)[MaxPop/2]) {
  int parentsIndices[MaxPop/2];
  for(int i = 0; i < population/2; i++) {
    for(int j = 0; j < parentsPerChild; j++)
      parentsIndices[j] = winners[rand() % (population/2)];
    make_child(parentsIndices, i);
  }
}

void GeneticAlg::make_child(const int (&parentsIndices)[MaxPop/2], const int & childIndex) {
  std::vector <double> parents[parentsPerChild];
  std::vector <double> child;
  std::vector <double> :: iterator it[parentsPerChild];
  double avWeight;
  for(int i = 0; i < parentsPerChild; i++) {
    parents[i] = NN[ parentsIndices[i] ]->vectorWeights();
    it[i] = parents[i].begin();
  }
  while(it[0] != parents[0].end()) {
    avWeight = 0;
    for(int i = 0; i < parentsPerChild; i++) {
      avWeight += *(it[i]);
    }
    for(int i = 0; i < parentsPerChild; i++) {
      ++(it[i]);
    }
    avWeight /= parentsPerChild;
    child.push_back(avWeight);
  }
  NN[childIndex]->updateWeights(child);
}

//mutations: addition
void GeneticAlg::mutate() {
  std::vector <double> popWeights[population];
  for(int i = 0; i < population; i++) {
    popWeights[i] = NN[i]->vectorWeights();
    std::vector<double>::iterator it;
    for(it = popWeights[i].begin(); it != popWeights[i].end(); ++it) {
      if( ((double)rand()) / ((double)RAND_MAX) < mutationRate)
        *it += (rand() % 2) ? mutation_add : -mutation_add;
    }
    NN[i]->updateWeights(popWeights[i]);
  }
}

/*

void GeneticAlg::selectAndMakeChildren(const int (&winners)[MaxPop/2], const int (&losers)[MaxPop/2]) {
  int childIndexes[MaxPop];
  int childCounter = 0;
  for(int i = 0; i < population/2; i+=parentsPerGen) {
    for(int j = 0; j < parentsPerGen; j++) {
      childIndexes[j] = winners[i+j];
      childIndexes[parentsPerGen+j] = losers[i+j];
    }//for
    makeOneGenChildren(childIndexes);
  }//for
}//selectAndMakeChildren


/*
void GeneticAlg::makeOneGenChildren(const int (&childIndexes)[MaxPop]) {
  std::vector <double> parents[parentsPerGen];
  std::vector <double> children[childrenPerGen];
  std::vector <double> :: iterator it[parentsPerGen];
  int childToNN;
  for(int i = 0; i < parentsPerGen; i++) {
    childToNN = childIndexes[i];
    parents[i] = NN[childToNN]->vectorWeights();
    it[i] = parents[i].begin();
  }//for
  int pickParent;
  while(it[0] != parents[0].end()) {
    for(int i = 0; i < childrenPerGen; i++) {
      pickParent = rand() % parentsPerGen;
      children[i].push_back(*(it[pickParent]));
    }//for
    for(int j = 0; j < parentsPerGen; j++)
      ++(it[j]);
  }//while
  for(int i = 0; i < childrenPerGen; i++) {
    mutateChild(children[i]);
    childToNN = (childrenPerGen>parentsPerGen)? childIndexes[i]:childIndexes[i+parentsPerGen];
    NN[childToNN]->updateWeights(children[i]);
  }//for
}//makeChildren

void GeneticAlg::mutateChild(std::vector <double> & childNN) {
  int avMutations = int(1/mutationRate);
  double multiplier;
  int randomMut;
  int counter = 0;
  int inputLength = NN[0]->getInputLength();
  for(std::vector <double> :: iterator it = childNN.begin(); it != childNN.end(); ++it) {
    while(*it > maxWeight || *it < -maxWeight)
      *it /= 2;
    while(*it < minWeight && *it > -minWeight)
      *it *= 2;
    if(rand() % avMutations == 0) {
      multiplier = (maxMutMult-1)*(((double)rand()) / ((double)RAND_MAX)) + 1;
      if(counter < inputLength)
        randomMut = rand () % 2;
      else
        randomMut = rand () % 4;
      switch(randomMut) {
        case 0:
          *it *= multiplier;
          break;
        case 1:
          *it /= multiplier;
          break;
        case 2:
          *it *= -multiplier;
          break;
        case 3:
          *it /= -multiplier;
          break;
      }//switch
    }//if
    counter++;
  }//for
}//mutateChild

*/

void GeneticAlg::NNfromFile() {
  std::string inputValue, fileName;
  std::vector <double> inputWeights;
  for(int i = 0; i < population; i++) {
    fileName = "NNs/file" + std::to_string(i) + ".txt";
    std::ifstream inputFile(fileName);
    if(inputFile.is_open()) {
      while(inputFile >> inputValue)
        inputWeights.push_back(std::stod(inputValue));
    }//if
    NN[i]->updateWeights(inputWeights);
    inputFile.close();
    inputWeights.clear();
  }//for
}//NNfromFile

void GeneticAlg::saveNN() {
  std::string fileName;
  for(int i = 0; i < population; i++) {
    fileName = "NNs/file" + std::to_string(i) + ".txt";
    std::ofstream outputFile(fileName);
    if(outputFile.is_open())
      outputFile << NN[i]->outputWeights();
    outputFile.close();
  }//for
}//initNN
