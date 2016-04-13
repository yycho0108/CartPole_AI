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

constexpr int ppow(int x, int n){ //positive power
	return n<1?1:x*ppow(x,n-1);
}
constexpr int log2(int x){
	return (x<2)?0:1+log2(x<<1);
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
private:
	Net<n*m,log2(ppow(10,n*m)),4> net; //subject to change
	double gamma; // gamma = reduction to future rewards
	double min_eps;
	std::deque<A_Memory> memories;
	int mSize; //memory size
	int rSize; //recall size = # samples to recall
	//input = 4x4 = 16 states + 4 actions
	//output = Q-value
public:
	Agent(int mSize=1, double gamma=0.8, double min_eps=0.05) //size of memory
		:net(0.99, 0.00001, 0.0001),gamma(gamma),min_eps(min_eps),mSize(mSize),rSize(1>mSize/3?1:mSize/3)
		//rho, eps, decay
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

		return maxDir;
	}
	double getMax(A_Board& board){
		//split this function as this serves an entirely new purpose...ish.
		double maxVal = 0.0;
		const bool* available = board.getAvailable();
		for(int i=0;i<4;++i){
			if(available[i]){
				auto nexti = board.speculate((DIR)i);
				//board.print();
				//namedPrint((DIR)i);
				//namedPrint(nexti);

				auto val = getMax(nexti);
				maxVal = maxVal>val?maxVal:val;
			}
		}
		return maxVal;
	}
	
	double getMax(std::vector<double>& state){// this is for probabilistic calculation
		double maxVal = 0.0;
		int empty=0;
		//namedPrint(state);
		for(auto& s : state){
			if(s == 0){ //= if empty
				++empty;
				s = 1/10.0;///1024.0;
				for(auto& e : net.FF(state)){ //iterate over all Q(S,A)
					auto val = 0.9 * e;
					maxVal = maxVal > val? maxVal : val;
				}

				s = 2/10.0;///1024.0;

				for(auto& e : net.FF(state)){
					auto val = 0.1 * e;
					maxVal = maxVal > val? maxVal : val;
				}

				s = 0; //back to 0
			}
		}
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
	void learn(A_Memory& memory, double alpha){
		const DIR a = memory.a;
		const double r = memory.r;
		auto& s = memory.s.vec(); 

		auto maxqn = getMax(memory.s_n);
		//namedPrint(SA);
		std::vector<double> y = net.FF(s); //old value
		//hline();
		//namedPrint(y);
		//auto oldy = y;
		//namedPrint(oldy);
		//y[(int)a] = (alpha)*y[(int)a] + (1-alpha)*(r+gamma*maxqn); //new value
		

		y[(int)a] = (1-alpha)*y[(int)a] + (alpha)*(r+gamma*maxqn); //new value

		//std::cout << "<[[" <<std::endl;
		
		//namedPrint(r);
		//namedPrint(maxqn);
	//	//
		
		//namedPrint(alpha);
		//namedPrint(y[0]);
		//namedPrint(y)
		net.BP(y);
		namedPrint(net.error());
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
	void update(A_Board& S, DIR a, double r, A_Board& next){
		//SARSA
		//State-Action, Reward, Max(next), alpha		
		memories.emplace_back(S, a, r,next);
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
		return net.FF(board.vec());
	}
	void print(){
		net.print();
	}
};
#endif
