#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <armadillo>
#include <vector>
#include <iostream>
using namespace arma;
double sigmoid(double);
vec sigmoid(vec&);
vec sigmoidPrime(vec&,bool);

template<typename T>
void print(std::vector<T> v){
	for(auto& e : v){
		std::cout << e << ", ";
	}
	std::cout << std::endl;
}

#define namedPrint(x) \
	std::cout << #x << " : " << x << endl;
#endif
