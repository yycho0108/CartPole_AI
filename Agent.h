#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include "Net.h"
#include "Board.h"
#include "Utility.h"

auto split = std::uniform_real_distribution<float>();
std::default_random_engine rng(time(0));

template<int n, int m>
class Agent{
private:
	float confidence;
	int age;
	Net net;
	std::vector<int> t;
	std::vector<double> v; //prevState
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(std::vector<int> t):net(t),t(t){
		age = 0;
		confidence = 0.1;	
	}
	//Agent Saving/Loading (to/from file) ... To Be Added
	DIR getRand(Board<n,m>& board){
		const bool* available = board.getAvailable();

		std::vector<DIR> av;
		for(int i=0;i<4;++i){
			if(available[i])
				av.push_back((DIR)i);
		}
		auto p = split(rng);

		return av[int(av.size()*p)];
	}	
	DIR getBest(Board<n,m>& board){
		//get best purely based on network feedforward q-value
		v= board.toVec();
		auto s = v.size();
		v.resize(s+4);//for 4 DIRs(RULD)

		//currently editing here
		float maxVal=-99999;
		DIR maxDir=X;

		const bool* available = board.getAvailable();
		
		for(int i=0;i<4;++i){ //or among available actions
			if(available[i]){
				v[s+i] = 1.0; //activate "action"
				auto val = net.FF(v)[0];
				if(val > maxVal){
					maxVal = val;
					maxDir = (DIR)i;
				}
				v[s+i] = 0.0;

			}
		}

		return maxDir;
	}
	float getMax(Board<n,m>& board){
		//split this function as this serves an entirely new purpose...ish.
		v= board.toVec();
		auto s = v.size();
		v.resize(s+4);//for 4 DIRs(RULD)

		//currently editing here
		float maxVal=0;

		const bool* available = board.getAvailable();
		for(int i=0;i<4;++i){ //or among available actions
			if(available[i]){
				v[s+i] = 1.0; //activate "action" (r/u/l/d)
				auto val = net.FF(v)[0];
				//namedPrint(val);
				maxVal = val>maxVal?val:maxVal;
				v[s+i] = 0.0; //undo activation
			}
		}
		return maxVal;
	}
	DIR getNext(Board<n,m>& board){
		return (split(rng) > confidence)? getRand(board) : getBest(board);
	}
	void update(DIR dir, double r, double qn){
		//print(v);
		//r = im. reward
		//qn = q_next (SARSA or Q-Learning)
		auto alpha = confidence;
		auto index = v.size()-4+(int)dir;
		v[index] = 1.0;

		std::vector<double> y = net.FF(v); //old value
		y[0] = (alpha)*y[0] + (1.0-alpha)*(r+qn); //new value

		//namedPrint(r);
		//namedPrint(qn);
		//namedPrint(y[0]);
		//namedPrint(alpha);

		net.BP(y);

		v[index] = 0.0;
		++age;
		confidence = tanh(age * 0.001);
	}
};

#endif
