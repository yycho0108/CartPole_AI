#include "Layer.h"

Layer::Layer(){

}

Layer::Layer(int n):n(n){
	setSize(n);
}

Layer::~Layer(){

}
void Layer::setSize(int n){
	_I.set_size(n);
	_O.set_size(n);
	_G.set_size(n);
}

void Layer::setT(tfun f, tfun_d f_d){
	this->f = f;
	this->f_d = f_d;
}

void Layer::transfer(mat v){
	_I.swap(v);
	//_O = _I;
	_O = f(_I);
	//cout << "I" << arma::size(_I) << endl;
	//cout << "O" << arma::size(_O) << endl;
//	_O.for_each([](mat::elem_type& val){val = sigmoid(val);});
	//return _O;
	//_I.for_each([]())
}
mat Layer::back_transfer(){
	return f_d(_O, true);
}
mat& Layer::I(){
	return _I;
}
mat& Layer::O(){
	return _O;
}
mat& Layer::G(){
	return _G;
}
