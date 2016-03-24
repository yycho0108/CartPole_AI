#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <armadillo>
using namespace arma;
double sigmoid(double);
vec sigmoid(vec&);
vec sigmoidPrime(vec&,bool);
#endif
