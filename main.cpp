#include "Params.h"
#include "GameManager.h"
#include <iostream>

using namespace std;

int main(){
	GameManager g("ai"); // 4x4 grid
	g.run();
	std::cout << "OVER" << std::endl;
}
