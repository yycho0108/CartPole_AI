
#include "Net.h"
#include "Utility.h"
#include <iostream>

#include <functional>
#include <ctime>
#include <random>



using namespace arma;
using namespace std;
Net::Net(std::vector<int> t, double alpha):t(t),alpha(alpha){
	for(size_t i=1;i<t.size();++i){
		W.push_back(arma::randn<mat>(t[i],t[i-1]));
		B.push_back(arma::randn<vec>(t[i]));
	}
	for(auto& e : t){
		L.push_back(Layer(e));
	}
}
std::vector<double> Net::FF(std::vector<double> X){
	L.front().O() = X;
	for(size_t i=1;i<t.size();++i){
		L[i].transfer(W[i-1]*L[i-1].O() + B[i-1]);	
	}
	return arma::conv_to<std::vector<double>>::from(L.back().O());
}
void Net::BP(std::vector<double> Y){
	L.back().G() = vec(Y) - L.back().O();
	for(size_t i = t.size()-2;i>=1;--i){
		L[i].G() = W[i].t() * L[i+1].G() % sigmoidPrime(L[i].O(),true);
	}
	for(size_t i=1;i<t.size();++i){
		//alpha = learning rate
		W[i-1] += alpha * L[i].G() * L[i-1].O().t();
		B[i-1] += alpha * L[i].G();
	}
}

double randNum(){
	static auto _randNum = std::bind(std::uniform_real_distribution<double>(0.0,1.0),std::default_random_engine(time(0))); //random
	return _randNum();
}

void XOR_GEN(std::vector<double>& X, std::vector<double>& Y){
	X[0] = randNum()>0.5?1:0;
	X[1] = randNum()>0.5?1:0;
	Y[0] = int(X[0]) ^ int(X[1]);
}
/*
int main(int argc, char* argv[]){
	int lim = 1000;
	if(argc != 1){
		lim = std::atoi(argv[1]);
	}
	std::vector<int> t({2,4,1});
	Net net(t);
	std::vector<double> X(2);
	std::vector<double> Y(1);

	auto start = clock();
	for(int i=0;i<lim;++i){
		XOR_GEN(X,Y);
		net.FF(X);
		net.BP(Y);
	}
	auto end = clock();
	printf("Took %f seconds", float(end-start)/CLOCKS_PER_SEC);
	for(int i=0;i<10;++i){
		XOR_GEN(X,Y);
		std::cout << X[0] << ',' << X[1] << ':' <<  Y[0] << '|' << net.FF(X)[0] << std::endl;
	}
}
*/
