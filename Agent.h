#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include <list>
#include "Net.h"
#include "Board.h"
#include "Utility.h"

auto split = std::uniform_real_distribution<float>();
std::default_random_engine rng(time(0));

template<int n>// n = state size
struct Memory{
	double sa[n+4];
	double r;
	double s_n[n+4]; //next state
	bool a_n[4]; //next availability
	Memory(std::vector<double>& sa, double r, std::vector<double>& s_n, const bool* a_n)
		:Memory(&sa.front(),r,&s_n.front(),a_n){
	}

	Memory(double* sa, double r, double* s_n, const bool* a_n){
		memcpy(this->sa,sa,(n+4)*sizeof(double));
		this->r = r;
		memcpy(this->s_n,s_n,n*sizeof(double));
		memset(this->s_n+n,0,sizeof(double)*4);
		memcpy(this->a_n,a_n,4*sizeof(bool));
	}
};

template<int n, int m>
class Agent{
	using A_Memory = Memory<n*m>;
	using A_Board = Board<n,m>;
private:
	float gamma; // gamma = 1 - confidence
	Net net;
	std::list<A_Memory> memories;
	int mSize;
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(std::vector<int> t, int mSize=1) //size of memory
		:net(t,0.6,0.001),mSize(mSize) //learning rate = 0.6, weight decay = 0.001
	{
		gamma = 0.8; //basically, how much discount by "time"? 
	}
	//Agent Saving/Loading (to/from file) ... To Be Added
	DIR getRand(A_Board& board){
		const bool* available = board.getAvailable();

		std::vector<DIR> av;
		for(int i=0;i<4;++i){
			if(available[i])
				av.push_back((DIR)i);
		}
		auto p = split(rng);

		return av[int(av.size()*p)];
	}	
	DIR getBest(A_Board& board){
		//get best purely based on network feedforward q-value
		std::vector<double> v= board.vec();
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
	float getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		std::vector<double> v = board.vec();
		const bool* available = board.getAvailable();
		return getMax(v,available);
	}
	float getMax(std::vector<double>& v,const bool* available){
		auto s = v.size();
		v.resize(s+4);//for 4 DIRs(RULD)

		//currently editing here
		float maxVal=-1;

		for(int i=0;i<4;++i){ //or among available actions
			if(available[i]){
				v[s+i] = 1.0; //activate "action" (r/u/l/d)
				//V = (S',A')
				auto val = net.FF(v)[0]; // Q(S',A')
				//namedPrint(val);
				maxVal = val>maxVal?val:maxVal;
				v[s+i] = 0.0; //undo activation
			}
		}
		return maxVal;
	}
	float getMax(const double* state, const bool* available){
		std::vector<double> v(state,state+n*m);
		return getMax(v,available);
	}
	DIR getNext(A_Board& board){
		//occasional random exploration
		//0.9 corresponds to "gamma" .. ish.
		return (split(rng) > 0.8)? getRand(board) : getBest(board);
	}
	void learn(A_Memory& memory, float alpha){
		std::vector<double> SA(memory.sa,memory.sa+n*m+4);
		const double r = memory.r;
		const double* s_n = memory.s_n;
		const bool* a_n = memory.a_n;

		auto qn = getMax(s_n,a_n);
		std::vector<double> y = net.FF(SA); //old value
		namedPrint(y[0]);
		y[0] = (alpha)*y[0] + (1.0-alpha)*(r+gamma*qn); //new value

		//std::cout << "<[[" <<std::endl;
		
		//namedPrint(r);
		//namedPrint(qn);
		
		//namedPrint(y[0]);

		net.BP(y);

	}
	void update(std::vector<double>& SA, double r,A_Board& next,float alpha){
		//SARSA
		//State-Action, Reward, Max(next), alpha
		memories.emplace_back(SA,r,next.vec(),next.getAvailable());
		if(memories.size() > mSize)
			memories.pop_front();

		learn(memories.back(), alpha);

	}
};

#endif
