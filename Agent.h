#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include <deque>
#include <cassert>
#include "Net.h"
#include "Board.h"
#include "Table.h"
#include "Utility.h"

auto split = std::uniform_real_distribution<double>();
std::default_random_engine rng(time(0));

constexpr long int ppow(int x, int n){ //positive power
	return n<1?1:x*ppow(x,n-1);
}
constexpr long int log2(int x){
	return (x<2)?0:1+log2(x>>1);
}

template<int n, int m, typename T=double>// n = state size
struct Memory{
	using A_Board = Board<n,m>;
	A_Board s;
	DIR a;
	double r;
	A_Board s_n;

	Memory(A_Board& s, DIR a, double r, A_Board& s_n)
	:s(s),a(a),r(r),s_n(s_n){

	}
};

template<int n, int m, typename T = double> //double/float = cannot use table, must use neural network
class Agent{
	using A_Memory = Memory<n,m,T>;
	using A_Board = Board<n,m>;
	static const int H = log2(ppow(10,n*m))/2;
private:
	Net<n*m,H,4> net; //subject to change
	double gamma; // gamma = reduction to future /ewards
	double min_eps;
	std::deque<A_Memory> memories;
	int mSize; //memory size
	int rSize; //recall size = # samples to recall
	bool _verbose;
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(int mSize=1, double gamma=0.8, double min_eps=0.05) //size of memory
		:net(0.9, 0.0001, 0.0001),gamma(gamma),min_eps(min_eps),mSize(mSize),rSize(1>mSize/3?1:mSize/3)
		//rho, eps, decay
		//learning rate = 0.3, weight decay = 0.001
	{
		_verbose = false;
		std::cout << H << std::endl;
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
		auto a = av[int(av.size())*p];
		assert(a != X);
		return a;// if av.size()==3 then ranges from 0~2.9999, end-exclusive
	}	

	DIR getBest(A_Board& board){
		//get best purely based on network feedforward q-value
		std::vector<double>& v= board.vec();
		//currently editing here
		double maxVal=-99999;//reasonably small value
		DIR maxDir=X;

		const bool* available = board.getAvailable();
		//namedPrint(v);

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

		assert(maxDir != X);
		return maxDir;
	}

	double getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		double maxVal = 0.0;
		const bool* available = board.getAvailable();
		auto s = board.vec();
		auto y = net.FF(s);
		for(int a=0;a<4;++a){
			if(available[a]){
				//auto nexti = board.speculate((DIR)i);
				//board.print();
				//namedPrint((DIR)i);
				//namedPrint(nexti);

				//auto val = getMax(nexti);
				auto val = y[a];
				maxVal = maxVal>val?maxVal:val;
			}
		}
		return maxVal;
	}
	
	double getMax(std::vector<double>& state){// this is for probabilistic calculation
		double maxVal = -99999;
		int empty=0;
		//namedPrint(state);
		for(auto& s : state){
			if(s == 0){ //= if empty
				++empty;
				s = 1/10.0;///fill with 2, div. 1024.0 normalization;
				for(auto& e : net.FF(state)){ //iterate over all Q(S,A)
					auto val = 0.9 * e;
					maxVal = maxVal > val? maxVal : val;
				}

				s = 2/10.0;///fill with 4, div. 1024.0;

				for(auto& e : net.FF(state)){
					auto val = 0.1 * e;
					maxVal = maxVal > val? maxVal : val;
				}

				s = 0; //back to 0
			}
		}
		if(empty == 0)
			return 0.0; //terminal state maxqn
		else
			return maxVal/empty;
	}

	DIR getNext(A_Board& board, double epsilon){
		epsilon = std::max(min_eps,epsilon); //0.05 = min_epsilon
		//namedPrint(epsilon);
		//occasional random exploration
		//0.9 corresponds to "gamma" .. ish.
		//e-greedy
		return (split(rng) < epsilon)? getRand(board) : getBest(board);
	}
	double learn(A_Memory& memory, double alpha){
		static std::vector<double> y;
		const DIR a = memory.a;
		const double r = memory.r;
		auto& s = memory.s.vec(); 

		auto maxqn = getMax(memory.s_n);
		//namedPrint(SA);
		y = net.FF(s);
		//std::vector<double> y = net.FF(s); //old value

		if(_verbose){
			hline();
			memory.s.print();
			namedPrint(y);
		}
		//auto oldy = y;
		//namedPrint(oldy);
		//namedPrint(alpha);
		//namedPrint(gamma);

		y[(int)a] = (1-alpha)*y[(int)a] + (alpha)*(r+gamma*maxqn); //new value

		//std::cout << "<[[" <<std::endl;
		
		if(_verbose){
			namedPrint(r);
			namedPrint(maxqn);
			namedPrint(y)
		}	
		net.BP(y);

		if(_verbose){
			cout << "-->" << endl;
			auto yp = net.FF(s);
			namedPrint(yp);
			/*
			//for debugging purposes...
			cout << "TESTING BP" << endl;
			for(int i=0;i<10;++i){
				namedPrint(net.FF(s));
				net.BP(y);
			}
			*/
		}
		
		return net.error();
	}
	double learn_bundle(double alpha){
		//static std::random_device rd;
		//static std::mt19937 eng(rd());
		//static std::uniform_int_distribution<int> distr(0,mSize);
		double sum = 0;
		for(int i=0;i<rSize;++i){
			//potentially replace with distinct random numbers
			//learn(memories[distr(eng)], alpha);
			sum += learn(memories[rand()%mSize],alpha);
		}
		return sum/rSize;
	}
	double learn_bundle(double alpha, int size){
		double sum = 0;
		for(int i=0;i<size;++i){
			sum += learn(memories[rand()%memories.size()],alpha);
		}
		return sum/size; //average
	}
	void memorize(A_Board& S, DIR a, double r, A_Board& next){
		//SARSA
		//State-Action, Reward, Max(next), alpha		
		memories.emplace_back(S, a, r, next);
		if(memories.size() > mSize){
			memories.pop_front();
		}
		//this->learn(memories.back(),alpha);

		//if(learn){
		//	if(memories.size() > mSize){
		//		memories.pop_front();
		//		learn_bundle(alpha);
		//	}else{
		//		auto size = std::min(rSize,(int)memories.size());
		//		learn_bundle(alpha,size);
		//	}
		//}
	}
	std::vector<double> guess(A_Board& board){
		auto& s = board.vec();
		return net.FF(s);
	}
	void print(){
		net.print();
	}
	bool& verbose(){
		return _verbose;
	}
};
#endif
