#ifndef __LAYER_H__
#define __LAYER_H__

#include <armadillo>
#include "Utility.h"

using namespace arma;

class Layer{
private:
	int n; //size
	vec _I,_O,_G;
	tfun f;
	tfun_d f_d;
public:
	Layer();
	Layer(int n);
	~Layer();
	void transfer(vec);
	vec& I();
	vec& O();
	vec& G();
	void setSize(int);
	void setT(tfun,tfun_d);
	int size();
};

#endif
