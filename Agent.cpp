#include "Agent.h"
#include <random>
#include <ctime>

auto dist = std::bernoulli_distribution(0.1);
std::default_random_engine rng(time(0));

Agent::Agent(std::vector<int> t):n(t){
	//needs implementation
}
DIR Agent::getRand(){
	//needs implementation
	return X;
}
DIR Agent::getBest(){
	//needs implementation
	return X;
}
DIR Agent::getNext(){
	return dist(rng)? getRand() : getBest();
}
