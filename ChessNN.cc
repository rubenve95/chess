#include <cmath>
#include <string>
#include <vector>
#include <ctime>         // for time stuff
#include <cstdlib>       // for rand ( )

const int fieldsPerSide = 8;
const int fieldValues = 13;  //12 pieces, 1 empty = 13 [-6,6]
const int inputs = fieldsPerSide*fieldsPerSide*fieldValues+1;  //833
const int maxHiddens = 25;

class ChessNN {
  public:

    ChessNN();

    std::string outputWeights();
    void updateWeights(const std::vector <double> & weights);
    std::vector <double> vectorWeights();
    int getNNLength();
    int getInputLength();
    void initWeights();

    double NeuralNet(const int (&board)[fieldsPerSide][fieldsPerSide], const int & color);
  private:
    double input[inputs];
    double output;
    double hidMid[maxHiddens];
    double hidOut[maxHiddens];
    double wIn[inputs][maxHiddens];
    double wHidMid[maxHiddens][maxHiddens];
    double wHidOut[maxHiddens];
    int hiddensMid;
    int hiddensOut;

    void printInp();
    double g (double x);
    void calcHidMid();
    void calcHidOut();
    void calcOutput();
    int translateOutput();
    void initInput();
    void setInput(const int (&board)[fieldsPerSide][fieldsPerSide], const int & color);
};//ChessNN

ChessNN::ChessNN() {
  hiddensMid = 20;
  hiddensOut = 5;
  input[0] = -1;
  hidMid[0] = -1;
  hidOut[0] = -1;
  //initWeights();
}//ChessNN constructor

/*
//sigmoid function
double ChessNN::g (double x) {
  return 1 / ( 1 + exp ( - x ) );
}//g
*/

//hyperbolic tangent
double ChessNN::g (double x) {
  return (1-exp(-2*x))/(1+exp(-2*x));
}//g

void ChessNN::initWeights() {
  for(int i = 0; i < hiddensMid; i++) {
    wHidOut[i] = ((double)rand()) / ((double)RAND_MAX) - rand () % 2;
    if(i > 0)
      for(int j = 0; j < inputs; j++) {
        wIn[j][i] = ((double)rand()) / ((double)RAND_MAX);
        if((j+1) % fieldValues < fieldValues/2)
          wIn[j][i]--;
        if(j < hiddensMid && i < hiddensOut)
          wHidMid[j][i] = ((double)rand()) / ((double)RAND_MAX) - rand () % 2;
    }//for
  }//for
}//initWeights

void ChessNN::calcHidMid() {
  for(int i = 1; i < hiddensMid; i++) {
    hidMid[i] = 0;
    for(int j = 0; j < inputs; j++)
      hidMid[i] += input[j]*wIn[j][i];
    hidMid[i] = g(hidMid[i]);
  }//for
}//calcHid

void ChessNN::calcHidOut() {
  for(int i = 1; i < hiddensOut; i++) {
    hidOut[i] = 0;
    for(int j = 0; j < hiddensMid; j++)
      hidOut[i] += hidMid[j]*wHidMid[j][i];
    hidOut[i] = g(hidOut[i]);
    //std::cout << hidOut[i] << " ";
  }//for
}//calcHidOut 

void ChessNN::calcOutput() {
  output = 0;
  for(int i = 0; i < hiddensOut; i++)
    output += hidOut[i]*wHidOut[i];
  output = g(output);
}//calcOutput

void ChessNN::initInput() {
  for(int i = 1; i < inputs; i++)
    input[i] = 0;
}//initInput

void ChessNN::setInput(const int (&board)[fieldsPerSide][fieldsPerSide], const int & color) {
  int inpInd = (fieldValues+1)/2;
  int piece;
  initInput();
  for(int i = 0; i < fieldsPerSide; i++) {
      for(int j = 0; j < fieldsPerSide; j++) {
        piece = color*board[i][j];
        input[inpInd+piece] = 1;
        inpInd += fieldValues;
      }//for
  }//for
}//setInput

void ChessNN::updateWeights(const std::vector <double> & weights) {
  std::vector <double>::const_iterator it;
  it = weights.begin();
  for(int i = 0; i < inputs; i++)
    for(int j = 1; j < hiddensMid; j++) {
      wIn[i][j] = *it;
      ++it;
    }//for

  for(int i = 0; i < hiddensMid; i++)
    for(int j = 1; j < hiddensOut; j++) {
      wHidMid[i][j] = *it;
      ++it;
    }//for

  for(int i = 0; i < hiddensOut; i++) {
      wHidOut[i] = *it;
      ++it;
    }//for
}//updateWeights

std::string ChessNN::outputWeights() {
  std::string weights = "";
  for(int i = 0; i < inputs; i++)
    for(int j = 1; j < hiddensMid; j++)
      weights += std::to_string(wIn[i][j]) + " ";
  weights += "\n";

  for(int i = 0; i < hiddensMid; i++)
    for(int j = 1; j < hiddensOut; j++)
      weights += std::to_string(wHidMid[i][j]) + " ";
  weights += "\n";

  for(int i = 0; i < hiddensOut; i++)
    weights += std::to_string(wHidOut[i]) + " ";
  weights += "\n";
  return weights;
}//printWeights

//gives number of weight connections
int ChessNN::getNNLength() {
  return inputs*hiddensMid+hiddensMid*hiddensOut+hiddensOut;
}//getNNLength

int ChessNN::getInputLength() {
  return inputs*hiddensMid;
}//getInputLength

std::vector <double> ChessNN::vectorWeights() {
  std::vector <double> weights;
  for(int i = 0; i < inputs; i++)
    for(int j = 1; j < hiddensMid; j++)
      weights.push_back(wIn[i][j]);
  for(int i = 0; i < hiddensMid; i++)
    for(int j = 1; j < hiddensOut; j++)
      weights.push_back(wHidMid[i][j]);
  for(int i = 0; i < hiddensOut; i++)
    weights.push_back(wHidOut[i]);
  return weights;
}//vectorWeights

//remove
void ChessNN::printInp() {
  for(int i = 0; i < inputs; i++)
    if(input[i])
      std::cout << i << " ";
  std::cout << std::endl << std::endl;
}//print

double ChessNN::NeuralNet(const int (&board)[fieldsPerSide][fieldsPerSide], const int & color) {
  setInput(board, color);
  calcHidMid();
  calcHidOut();
  calcOutput();
  return output;
}//NeuralNet
