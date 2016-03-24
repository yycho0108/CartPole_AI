#ifndef __NET_H__
#define __NET_H__

#include "Layer.h"
#include <vector>
#include <armadillo>

using namespace arma;

class Net{
private:
	std::vector<int> t; //topology
	std::vector<mat> W;
	std::vector<Layer> L; //layers
	std::vector<vec> B; //biases
public:
	std::vector<double> FF(std::vector<double> X);
	void BP(std::vector<double> Y);
	Net(std::vector<int> t);
};


#endif
