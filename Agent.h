#ifndef __AGENT_H__
#define __AGENT_H__

#include "Net.h"
#include "Board.h"

class Agent{
private:
	Net n;
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(std::vector<int> t);
	DIR getRand();
	DIR getBest();
	DIR getNext();
};

#endif
