/* ** */
#include "GameManager.h"
#include <iostream>

using namespace std;


/* TEST NET */
#include "Net.h"

//int main(int argc, char* argv[]){
//	int lim = 1000;
//	if(argc != 1){
//		lim = std::atoi(argv[1]);
//	}
//	Net<2,4,1> net(0.6,0.001); //for learning rate & decay use default
//	std::vector<double> X(2);
//	std::vector<double> Y(1);
//
//	auto start = clock();
//	for(int i=0;i<lim;++i){
//		XOR_GEN(X,Y);
//		net.FF(X);
//		net.BP(Y);
//	}
//	auto end = clock();
//	printf("Took %f seconds", float(end-start)/CLOCKS_PER_SEC);
//	for(int i=0;i<10;++i){
//		XOR_GEN(X,Y);
//		std::cout << X[0] << ',' << X[1] << ':' <<  Y[0] << '|' << net.FF(X)[0] << std::endl;
//	}
//}


int main(int argc, char* argv[]){
	int max_epoch = 500;
	if(argc != 1){
		max_epoch = std::atoi(argv[1]);
	}
	GameManager<4,4> g("ai",max_epoch);
	g.run();
}

