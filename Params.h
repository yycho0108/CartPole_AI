#ifndef __PARAMS_H__
#define __PARAMS_H__

// G = global
#define G extern const

#define WIDTH 4
#define HEIGHT 4

G double ALPHA;
G double GAMMA;
G double MIN_EPS;
G double WEIGHT_DECAY;

G int MEM_SIZE;
G int MAX_EPOCH;
G int U_FREQ;
G int NUM_TEST;

G bool VERBOSE;


#undef G

#endif
