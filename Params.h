#ifndef __PARAMS_H__
#define __PARAMS_H__

// G = global
#define G extern const

#define WIDTH 4
#define HEIGHT 4

#define N_IN 4
#define N_OUT 2

G double ALPHA;
G double GAMMA;
G double MIN_EPS;
G double RHO;
G double WEIGHT_DECAY;

G int MEM_SIZE;
G int MAX_EPOCH;
G int U_START;
G int U_FREQ;
G int U_SIZE;
G int NUM_TEST;

G bool VERBOSE;

enum:char{SIGM,TANH,RELU,LIN};

enum:char{SGD,ADADELTA,RMSPROP};

G char ACTIVATION;
G char OPTIMIZER;

#undef G

#endif
