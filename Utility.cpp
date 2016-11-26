#include "Utility.h"

double sigmoid(double x){
	//return tanh(x);
	return 1.0/(1.0 + exp(-x));
}
mat sigmoid(mat& v){
	//return tanh(v);
	return 1.0/(1.0 + exp(-v));
}
mat sigmoidPrime(mat& v, bool sig){
	if(sig){
		//return 1.0 - (v%v);
		return v % (1.0-v);
	}
	else{
		mat s = sigmoid(v);
		//return 1.0 - (s%s);
		return s % (1.0-s);
	}
}
mat tanh(mat& v){
	return arma::tanh(v);
}
mat tanhPrime(mat& v, bool sig){
	if(sig){
		return 1.0 - (v%v);
	}else{
		mat s = tanh(v);
		return s % (1.0-s);
	}
}

double relu(double& v){
	return v>0?v:0;
}
double reluPrime(double& v){
	return v>0?1:0;
}

mat relu(mat& v){
	mat res = v;
	res.transform([](double v){return v>0?v:0;});
	return res;
}

mat reluPrime(mat& v, bool){
	mat res = v;
	res.transform([](double v){return v>0?1:0;});
	return res;
}

mat linear(mat& v){
	return v;
}
mat linearPrime(mat& v, bool){
	mat res;
	res.set_size(v.size());
	res.fill(1.0);
	return res;
}

mat softplus(mat& v){
	return log(1 + exp(v));
}

mat softplusPrime(mat& v){
	return sigmoid(v);
}

mat softmax(mat& v){
	float m = arma::max(arma::max(v));
	mat e = exp(v - m);
	float s = arma::sum(sum(e));

	if(s)
		e /= s;

	return e;
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

float bound(float mn, float x, float mx){
	if(x < mn)
		return mn;
	if(x > mx)
		return mx;
	return x;
}

float within(float mn, float x, float mx){
	return (mn < x && x < mx);
}


