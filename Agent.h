#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include <deque>
#include "Net.h"
#include "Board.h"
#include "Table.h"
#include "Utility.h"

auto split = std::uniform_real_distribution<double>();
std::default_random_engine rng(time(0));

template<int n, typename T=double>// n = state size
struct Memory{
	T s[n];
	DIR a;
	double r;
	T s_n[n]; //next state
	bool a_n[4]; //next availability
	Memory(std::vector<T>& s, DIR a, double r, std::vector<T>& s_n, const bool* a_n)
		:Memory(&s.front(),a,r,&s_n.front(),a_n){
	} //delegating constructor

	Memory(T* s, DIR a, double r, T* s_n, const bool* a_n){
		memcpy(this->s,s,n*sizeof(T));
		this->a = a;
		this->r = r;
		memcpy(this->s_n,s_n,n*sizeof(T));
		//memset(this->s_n+n,0,4*sizeof(double));
		memcpy(this->a_n,a_n,4*sizeof(bool));
	}
};

template<int n, int m, typename T = double> //double/float = cannot use table, must use neural network
class Agent{
	using A_Memory = Memory<n*m,T>;
	using A_Board = Board<n,m>;
private:
	Net<n*m, n*m/2, 4> net; //subject to change
	double gamma; // gamma = reduction to future rewards
	double min_eps;
	std::deque<A_Memory> memories;
	int mSize; //memory size
	int rSize; //recall size = # samples to recall
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(int mSize=1, double gamma=0.8, double min_eps=0.05) //size of memory
		:net(0.6,0.001),gamma(gamma),min_eps(min_eps),mSize(mSize),rSize(1>mSize/3?1:mSize/3)
		//learning rate = 0.3, weight decay = 0.001
	{
		/*std::vector<double> v(n*m+4);
		for(int i=0;i<4;++i){
			v[n*m+i] = 1;
			auto val = net.FF(v);
			namedPrint(val);
			v[n*m+i] = 0;
		}*/

	}
	//Agent Saving/Loading (to/from file) ... To Be Added
	DIR getRand(A_Board& board){
		const bool* available = board.getAvailable();

		std::vector<DIR> av; //list of available actions
		for(int i=0;i<4;++i){
			if(available[i]){
				auto dir = (DIR)i;
				av.push_back(dir);
			}
		}
		auto p = split(rng);

		return av[int(av.size()*p)];// if av.size()==3 then ranges from 0~2.9999, end-exclusive
	}	
	DIR getBest(A_Board& board){
		//get best purely based on network feedforward q-value
		std::vector<double>& v= board.vec();
		//currently editing here
		double maxVal=-99999;
		DIR maxDir=X;

		const bool* available = board.getAvailable();

		auto a = net.FF(v);
		for(int i=0;i<4;++i){ 
			if(available[i]){ //among available actions
				//namedPrint(val);
				if(a[i] > maxVal){ //this is why R was favorite
					maxVal = a[i];
					maxDir = (DIR)i;
				}
			}
		}
		//namedPrint(maxDir);

		return maxDir;
	}
	double getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		std::vector<double>& v = board.vec();
		const bool* available = board.getAvailable();
		return getMax(v,available);
	}
	double getMax(std::vector<double>& v,const bool* available){
		//currently editing here
		double maxVal = 0.0; //what should this be initialied to?
		//(represents value when nothing is available)
		
		auto a = net.FF(v);

		for(int i=0;i<4;++i){
			if(available[i]){	// among available actions
				if(a[i]>maxVal){
					maxVal = a[i];
				}
			}
		}
		return maxVal;
	}
	double getMax(const double* state, const bool* available){
		std::vector<double> v(state,state+n*m);
		return getMax(v,available);
	}
	DIR getNext(A_Board& board, double epsilon){
		epsilon = std::max(min_eps,epsilon); //0.05 = min_epsilon
		//namedPrint(epsilon);
		//occasional random exploration
		//0.9 corresponds to "gamma" .. ish.
		//e-greedy
		return (split(rng) < epsilon)? getRand(board) : getBest(board);
	}
	void learn(A_Memory& memory, double alpha){
		std::vector<double> s(memory.s,memory.s+n*m);
		const DIR a = memory.a;
		const double r = memory.r;
		const double* s_n = memory.s_n;
		const bool* a_n = memory.a_n;

		auto maxqn = getMax(s_n,a_n);

		//namedPrint(SA);
		std::vector<double> y = net.FF(s); //old value
		//hline();
		//namedPrint(y);
		//auto oldy = y[0];
		//y[(int)a] = (alpha)*y[(int)a] + (1-alpha)*(r+gamma*maxqn); //new value

		y[(int)a] = (1-alpha)*y[(int)a] + (alpha)*(r+gamma*maxqn); //new value

		//std::cout << "<[[" <<std::endl;
		
		//namedPrint(r);
		//namedPrint(maxqn);
		//
		
		//namedPrint(alpha);
		//namedPrint(y[0]);
		//auto dy = y[0]-oldy;
		//namedPrint(dy/y[0]);
		//namedPrint(y)
		net.BP(y);
		//cout << "--> " << endl;

		//y = net.FF(s);
		//namedPrint(y);


	}
	void learn_bundle(double alpha){
		//static std::random_device rd;
		//static std::mt19937 eng(rd());
		//static std::uniform_int_distribution<int> distr(0,mSize);
		for(int i=0;i<rSize;++i){
			//potentially replace with distinct random numbers
			//learn(memories[distr(eng)], alpha);
			learn(memories[rand()%mSize],alpha);
		}
	}
	void learn_bundle(double alpha, int size){
		for(int i=0;i<size;++i){
			learn(memories[rand()%memories.size()],alpha);
		}
	}
	void update(std::vector<double>& S, DIR a, double r,A_Board& next,double alpha, bool learn=true){
		//SARSA
		//State-Action, Reward, Max(next), alpha		
		memories.emplace_back(S, a, r,next.vec(),next.getAvailable());
		//this->learn(memories.back(),alpha);

		if(learn){
			if(memories.size() > mSize){
				memories.pop_front();
				learn_bundle(alpha);
			}else{
				auto size = std::min(rSize,(int)memories.size());
				learn_bundle(alpha,size);
			}
		}
	}

	std::vector<double> guess(A_Board& board){
		return net.FF(board.vec());
	}
};

template<int n, int m>
class Agent<n,m, char>{ //partial specialization for table-agent
	using A_Memory = Memory<n*m, char>;
	using A_Board = Board<n,m>;
private:
	Table<n*m> table;
	double gamma;
	double min_eps;
	std::deque<A_Memory> memories;
	int mSize; //memory size
	int rSize; //recall size = # samples to recall
public:
	Agent(int mSize=1, double gamma=0.8, double min_eps=0.05) //size of memory
		:gamma(gamma),min_eps(min_eps),mSize(mSize),rSize(1>mSize/3?1:mSize/3){

		}
		//learning rate = 0.3, weight decay = 0.001
	DIR getRand(A_Board& board){
		const bool* available = board.getAvailable();

		std::vector<DIR> av; //list of available actions
		for(int i=0;i<4;++i){
			if(available[i]){
				auto dir = (DIR)i;
				av.push_back(dir);
			}
		}
		auto p = split(rng);

		return av[int(av.size()*p)];// if av.size()==3 then ranges from 0~2.9999, end-exclusive
	}	
	DIR getBest(A_Board& board){
		//get best purely based on table q-value
		std::vector<char>& v= board.cVec();
		//currently editing here
		double maxVal=-99999;
		DIR maxDir=X;

		const bool* available = board.getAvailable();

		auto a = table.FF(v);
		for(int i=0;i<4;++i){ 
			if(available[i]){ //among available actions
				//namedPrint(val);
				if(a[i] > maxVal){ //this is why R was favorite
					maxVal = a[i];
					maxDir = (DIR)i;
				}
			}
		}
		//namedPrint(maxDir);

		return maxDir;
	}
	double getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		std::vector<char>& v = board.cVec();
		const bool* available = board.getAvailable();
		return getMax(v,available);
	}
	double getMax(std::vector<char>& v,const bool* available){
		//currently editing here
		double maxVal = 0.0; //what should this be initialied to?
		auto a = table.FF(v);

		for(int i=0;i<4;++i){
			if(available[i]){	// among available actions
				if(a[i]>maxVal){
					maxVal = a[i];
				}
			}
		}
		return maxVal;
	}
	double getMax(const char* state, const bool* available){
		std::vector<char> v(state,state+n*m);
		return getMax(v,available);
	}
	DIR getNext(A_Board& board, double epsilon){
		epsilon = std::max(min_eps,epsilon); //0.05 = min_epsilon
		//namedPrint(epsilon);
		//occasional random exploration
		//0.9 corresponds to "gamma" .. ish.
		//e-greedy
		return (split(rng) < epsilon)? getRand(board) : getBest(board);
	}
	void learn(A_Memory& memory, double alpha){
		std::vector<char> s(memory.s,memory.s+n*m);
		const DIR a = memory.a;
		const double r = memory.r;
		const char* s_n = memory.s_n;
		const bool* a_n = memory.a_n;

		auto maxqn = getMax(s_n,a_n);
		//namedPrint(SA);
		auto y = table.FF(s); //old value
		//hline();
		//namedPrint(y);
		auto oldy = y[0];
		y[(int)a] = (1-alpha)*y[(int)a] + (alpha)*(r+gamma*maxqn); //new value

		//std::cout << "<[[" <<std::endl;
		
		namedPrint(r);
		namedPrint(maxqn);
		//namedPrint(alpha);
		
		//namedPrint(y[0]);
		auto dy = y[0]-oldy;
		namedPrint(dy);
		//namedPrint(y)
		table.BP(y);
		//cout << "--> " << endl;

		//y = table.FF(s);
		//namedPrint(y);
	}

	void learn_bundle(double alpha){
		//static std::random_device rd;
		//static std::mt19937 eng(rd());
		//static std::uniform_int_distribution<int> distr(0,mSize);
		for(int i=0;i<rSize;++i){
			//potentially replace with distinct random numbers
			//learn(memories[distr(eng)], alpha);
			learn(memories[rand()%mSize],alpha);
		}
	}
	void learn_bundle(double alpha, int size){
		for(int i=0;i<size;++i){
			learn(memories[rand()%memories.size()],alpha);
		}
	}
	
	void setPrev(A_Board& prev){
		std::vector<char> s = prev.cVec();
	}

	void update(std::vector<char>& S, DIR a, double r, A_Board& next,double alpha, bool learn=true){
		//SARSA
		//State-Action, Reward, Max(next), alpha		
		memories.emplace_back(S, a, r,next.cVec(),next.getAvailable());
		//this->learn(memories.back(),alpha);

		if(learn){
			if(memories.size() > mSize){
				memories.pop_front();
				learn_bundle(alpha);
			}else{
				auto size = std::min(rSize,(int)memories.size());
				learn_bundle(alpha,size);
			}
		}
	}
	void printTableSize(){
		namedPrint(table.size());
	}

	std::vector<double> guess(A_Board& board){
		return table.FF(board.vec());
	}
};

#endif
