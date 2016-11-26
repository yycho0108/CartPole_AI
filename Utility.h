#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <armadillo>
#include <vector>
#include <iostream>
using namespace arma;
double sigmoid(double);

mat sigmoid(mat&);
mat sigmoidPrime(mat&,bool);

mat tanh(mat&);
mat tanhPrime(mat&, bool);

mat relu(mat&);
mat reluPrime(mat&, bool);

mat linear(mat&);
mat linearPrime(mat&, bool);

mat softmax(mat&);
mat softmaxPrime(mat&, bool);

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
using tfun = mat (*)(mat&);
using tfun_d = mat (*)(mat&,bool);

struct Randomizer{
	std::random_device rd;
	std::mt19937 eng;
	std::uniform_real_distribution<float> dist;

	Randomizer(float min, float max):
		eng(rd()), dist(min,max){
		}
	float operator()(){
		return dist(eng);
	}

	template<typename T>
		void operator()(std::vector<T>& v){
			// fill vector
			for(auto& e : v){
				e = (*this)();
			}
		}

	template<typename T>
		void operator()(T& e){
			e = (*this)();
		}
	template<typename H, typename... T>
		void operator()(H& head, T... tail){
			(*this)(head);
			(*this)(tail...);
		}
};

extern float bound(float,float,float);
extern float within(float,float,float);

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define namedPrint(x) \
	std::cout << #x << " : " << x << std::endl;

#define alert() \
	std::cout << "HERE AT [" << __FILENAME__ << ':' << __LINE__ << ']' << std::endl;

#endif

