#include "Params.h"

// width and height of grid
#define C const

C double ALPHA = 1; // learning rate
C double GAMMA = 0.99;
C double RHO = 0.95;
C double MIN_EPS = 0.05; // minimum epsilon (exploration)
C double WEIGHT_DECAY = 0.001; // weight decay factor
C int MEM_SIZE = 5000;
C int MAX_EPOCH = 5000; // max epoch

C int U_SIZE = 50;
C int U_FREQ = 10; // update frequency
C int U_START = 100;

C int NUM_TEST = 100;
C bool VERBOSE = false;

C char ACTIVATION = RELU;
C char OPTIMIZER = RMSPROP;

#undef C
