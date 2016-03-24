#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include "Net.h"
#include "Board.h"

auto dist = std::bernoulli_distribution(0.1);
auto split = std::uniform_real_distribution<float>();
std::default_random_engine rng(time(0));

template<int n, int m>
class Agent{
private:
	Net net;
	std::vector<int> t;
	char tboard[n][m];
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(std::vector<int> t):net(t),t(t){
		
	}
	DIR getRand(){
			auto p = split(rng);
		if(p < 0.25){
			return R;
		}else if (p < 0.5){
			return U;
		}else if (p < 0.75){
			return L;
		}else{
			return D;
		}
		//needs implementation
		return X;	
	}
	DIR getBest(Board<n,m>& board){
		for(int i=0;i<4;++i){
			board.ifnext(DIR(i), tboard);
		}
		return X;
	}
	DIR getNext(Board<n,m>& board){
		return dist(rng)? getRand() : getBest(board);
	}
};

#endif
