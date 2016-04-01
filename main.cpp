/* ** */
#include "GameManager.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
	int max_epoch = 500;
	if(argc != 1){
		max_epoch = std::atoi(argv[1]);
	}
	GameManager<4,4> g("ai");
	g.run(max_epoch);
}
