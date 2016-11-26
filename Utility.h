#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <armadillo>
#include <vector>
#include <iostream>
using namespace arma;
double sigmoid(double);

vec sigmoid(vec&);
vec sigmoidPrime(vec&,bool);

vec tanh(vec&);
vec tanhPrime(vec&, bool);

vec relu(vec&);
vec reluPrime(vec&, bool);

template<typename T>
std::ostream& operator<<(std::ostream& o, std::vector<T> v){
	o << '[';
	for(auto& e : v){
		o << e << ", ";
	}
	o << ']';
	return o << std::endl;
}

extern void hline();
extern void checkPoint(std::string s="");

extern bool prompt(std::string query);
using tfun = vec (*)(vec&);
using tfun_d = vec (*)(vec&,bool);


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define namedPrint(x) \
	std::cout << #x << " : " << x << std::endl;

#define alert() \
	std::cout << "HERE AT [" << __FILENAME__ << ':' << __LINE__ << ']' << std::endl;

#endif

