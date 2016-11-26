#ifndef __AGENT_H__
#define __AGENT_H__

#include <random>
#include <ctime>
#include <deque>
#include <cassert>
#include "CartPole.h"
#include "Net.h"
#include "Utility.h"
#include "Params.h"

auto split = std::uniform_real_distribution<double>();
std::default_random_engine rng(time(0));

constexpr long int ppow(int x, int n){ //positive power
	return n<1?1:x*ppow(x,n-1);
}
constexpr long int my_log2(int x){
	return (x<2)?0:1+log2(x>>1);
}

struct Memory{
	CartPoleState s;
	double a;
	double r;
	CartPoleState s_n;

	Memory(CartPoleState& s, double a, double r, CartPoleState& s_n)
	:s(s),a(a),r(r),s_n(s_n){

	}
};

class Agent{

private:
	Net<N_IN,16,16,16,N_OUT> net; //subject to change
	Net<N_IN,16,16,16,N_OUT> target_net; //target network

	std::deque<Memory> memories;
	int rSize; //recall size = # samples to recall
public:
	Agent() //size of memory
		:net(RHO, 1e-6, WEIGHT_DECAY),rSize(U_SIZE)
		//rho, eps, decay
		//learning rate = 0.3, weight decay = 0.001
	{
		net.copyTo(target_net); // same weights for starters
	}

	//Agent Saving/Loading (to/from file) ... To Be Added
	float getRand(){
		// return index, not action
		static Randomizer r(0.0, N_OUT);
		return int(r());
	}	

	std::pair<float,float> getBest(const CartPoleState& state){
		//get best purely based on network feedforward q-value
		std::vector<double> v = state.vec();
		//currently editing here
		double maxVal = -99999;//reasonably small value
		double maxAction = 0.0;

		auto a = target_net.FF(v);
		for(int i=0;i<3;++i){ 
			if(a[i] > maxVal){ //this is why R was favorite
				maxVal = a[i];
				maxAction = i;
			}
		}
		return std::make_pair(maxVal, maxAction);
	}

	float getNext(const CartPoleState& state, double epsilon){
		epsilon = std::max(MIN_EPS,epsilon); //0.05 = MIN_EPSilon
		float best_action = getBest(state).second;
		return (split(rng) < epsilon)? getRand() : best_action;
	}

	double learn(Memory& memory, double alpha){
		static std::vector<double> y;

		const double a = memory.a;
		const double r = memory.r;

		auto s = memory.s.vec(); 

		auto best = getBest(memory.s_n);
		auto maxqn = best.first;

		// stabilizing with target network
		//
		y = target_net.FF(s); // compute target

		y[(int)a] = (1-alpha)*y[(int)a] + (alpha)*(r+GAMMA*maxqn); //fill with new value

		net.FF(s); // compuete output
		net.BP(y); // back-propagate against target

		return net.error();
	}

	double learn_bundle(double alpha){
		// TODO : convert to minibatch
		static std::random_device rd;
		static std::mt19937 eng(rd());
		static std::uniform_int_distribution<int> distr(0,MEM_SIZE);

		double sum = 0;
		for(int i=0;i<rSize;++i){
			sum += learn(memories[distr(eng)], alpha);
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

	void memorize(CartPoleState& S, float a, double r, CartPoleState& next){
		//SARSA
		//State-Action, Reward, Max(next), alpha		
		memories.emplace_back(S, a, r, next);
		if(memories.size() > MEM_SIZE){
			memories.pop_front();
		}
	}

	void freeze(){
		net.copyTo(target_net);
	}

	std::vector<double> guess(const CartPoleState& state){
		auto s = state.vec();
		return net.FF(s);
	}

	void print(){
		net.print();
	}
};
#endif
