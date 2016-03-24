#ifndef __LAYER_H__
#define __LAYER_H__

#include <armadillo>
using namespace arma;

class Layer{
private:
	int n; //size
	vec _I,_O,_G;
public:
	Layer(int n);
	~Layer();
	void transfer(vec);
	vec& I();
	vec& O();
	vec& G();
	int size();
};

#endif
