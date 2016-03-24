#ifndef __AGENT_H__
#define __AGENT_H__

#include "Net.h"

class Agent{
private:
	Net n;
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(std::vector<int> t);
	int getRand();
	int getBest();
	int getNext();
};

#endif
