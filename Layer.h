#ifndef __LAYER_H__
#define __LAYER_H__

#include <armadillo>
#include "Utility.h"

using namespace arma;

class Layer{
private:
	int n; //size
	mat _I,_O,_G;
	tfun f;
	tfun_d f_d;
public:
	Layer();
	Layer(int n);
	~Layer();
	void transfer(mat);
	mat back_transfer();
	mat& I();
	mat& O();
	mat& G();
	void setSize(int);
	void setT(tfun,tfun_d);
	int size();
};

#endif
