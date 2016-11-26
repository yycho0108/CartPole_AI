#include "Params.h"

// width and height of grid
#define C const

C double ALPHA = 0.01;
C double GAMMA = 0.9;
C double MIN_EPS = 0.05; // minimum epsilon
C double WEIGHT_DECAY = 0.001; // weight decay factor
C int MEM_SIZE = 1000;
C int MAX_EPOCH = 5000; // max epoch
C int U_FREQ = 100; // update frequency
C int NUM_TEST = 100;
C bool VERBOSE = false;

#undef C
