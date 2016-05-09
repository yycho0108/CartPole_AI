#include "Utility.h"

double sigmoid(double x){
	//return tanh(x);
	return 1.0/(1.0 + exp(-x));
}
vec sigmoid(vec& v){
	//return tanh(v);
	return 1.0/(1.0 + exp(-v));
}
vec sigmoidPrime(vec& v, bool sig){
	if(sig){
		//return 1.0 - (v%v);
		return v % (1.0-v);
	}
	else{
		vec s = sigmoid(v);
		//return 1.0 - (s%s);
		return s % (1.0-s);
	}
}
vec tanh(vec& v){
	return arma::tanh(v);
}
vec tanhPrime(vec& v, bool sig){
	if(sig){
		return 1.0 - (v%v);
	}else{
		vec s = tanh(v);
		return s % (1.0-s);
	}
}

void hline(){
	cout << "-------------------------" << endl;
}

bool prompt(std::string query){
	std::cin.clear();

	std::cout << query << std::endl;
	char ans;
	std::cin >> ans;
	if(ans == 'y' || ans == 'Y')
		return true;
	return false;
}
