#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include <deque>
#include "Net.h"
#include "Board.h"
#include "Utility.h"

auto split = std::uniform_real_distribution<double>();
std::default_random_engine rng(time(0));

template<int n>// n = state size
struct Memory{
	double sa[n+4];
	double r;
	double s_n[n+4]; //next state
	bool a_n[4]; //next availability
	Memory(std::vector<double>& sa, double r, std::vector<double>& s_n, const bool* a_n)
		:Memory(&sa.front(),r,&s_n.front(),a_n){
	} //delegating constructor

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
	Net<n*m+4, n*m/2, n*m/2, 1> net; //subject to change
	double gamma; // gamma = 1 - confidence
	double epsilon;
	std::deque<A_Memory> memories;
	int mSize; //memory size
	int rSize; //recall size = # samples to recall
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(int mSize=1, double gamma=0.8, double epsilon=0.05) //size of memory
		:net(0.3,0.001),gamma(gamma),epsilon(epsilon),mSize(mSize),rSize(1>mSize/3?1:mSize/3)
		//learning rate = 0.6, weight decay = 0.001
	{
	}
	//Agent Saving/Loading (to/from file) ... To Be Added
	DIR getRand(A_Board& board){
		const bool* available = board.getAvailable();

		std::vector<DIR> av; //list of available actions
		for(int i=0;i<4;++i){
			if(available[i])
				av.push_back((DIR)i);
		}
		auto p = split(rng);

		return av[int(av.size()*p)];// if av.size()==3 then ranges from 0~2.9999, end-exclusive
	}	
	DIR getBest(A_Board& board){
		//get best purely based on network feedforward q-value
		std::vector<double> v= board.vec();
		auto s = v.size();
		v.resize(s+4);//for 4 DIRs(RULD)

		//currently editing here
		double maxVal=-99999;
		DIR maxDir=X;

		const bool* available = board.getAvailable();
		
		for(int i=0;i<4;++i){ 
			if(available[i]){ //among available actions
				v[s+i] = 1.0; //activate "action"
				auto val = net.FF(v)[0];
				//namedPrint(val);
				if(val > maxVal){
					maxVal = val;
					maxDir = (DIR)i;
				}
				v[s+i] = 0.0;

			}
		}

		return maxDir;
	}
	double getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		std::vector<double> v = board.vec();
		const bool* available = board.getAvailable();
		return getMax(v,available);
	}
	double getMax(std::vector<double>& v,const bool* available){
		auto s = v.size();
		v.resize(s+4);//for 4 DIRs(RULD)

		//currently editing here
		double maxVal=-1.0;

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
		namedPrint(maxVal);
		return maxVal;
	}
	double getMax(const double* state, const bool* available){
		std::vector<double> v(state,state+n*m);
		return getMax(v,available);
	}
	DIR getNext(A_Board& board){
		//occasional random exploration
		//0.9 corresponds to "gamma" .. ish.
		//e-greedy
		return (split(rng) < epsilon)? getRand(board) : getBest(board);
	}
	void learn(A_Memory& memory, double alpha){
		std::vector<double> SA(memory.sa,memory.sa+n*m+4);
		const double r = memory.r;
		const double* s_n = memory.s_n;
		const bool* a_n = memory.a_n;

		auto qn = getMax(s_n,a_n);
		std::vector<double> y = net.FF(SA); //old value
		y[0] = (alpha)*y[0] + (1.0-alpha)*(r+gamma*qn); //new value

		//std::cout << "<[[" <<std::endl;
		
		//namedPrint(r);
		//namedPrint(qn);
		
		//namedPrint(y[0]);

		net.BP(y);

	}
	void learn_batch(double alpha){
		static std::random_device rd;
		static std::mt19937 eng(rd());
		static std::uniform_int_distribution<int> distr(0,mSize);

		for(int i=0;i<rSize;++i){
			//potentially replace with distinct random numbers
			learn(memories[distr(eng)], alpha);
		}
	}
	void update(std::vector<double>& SA, double r,A_Board& next,double alpha){
		//SARSA
		//State-Action, Reward, Max(next), alpha
		memories.emplace_back(SA,r,next.vec(),next.getAvailable());

		if(memories.size() > mSize)
			memories.pop_front();

		if(mSize == memories.size())
			learn_batch(alpha);
		else
			learn(memories.back(), alpha);
	}
};

#endif
