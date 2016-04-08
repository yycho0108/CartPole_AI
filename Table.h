#ifndef __TABLE_H__
#define __TABLE_H__

#include "Utility.h"
#include <unordered_map>

template<int n>
struct Key{
	char p[n];
	Key(){

	}
	explicit Key(char* p){
		memcpy(this->p,p,n * sizeof(char));
	}
	bool operator==(const Key<n>& k) const{
		for(int i=0;i<n;++i){
			if(p[i] != k.p[i])
				return false;
		}
		return true;
	}

};

namespace std{
	template <int n>
	struct hash<Key<n>>{
		std::size_t operator()(const Key<n>& k) const{
			std::size_t sum = 0;
			for(int i=0;i<n;++i){
				sum += k.p[n];
			}
			return sum;
		}
	};
}

template<int n>
class Table{
	std::unordered_map<Key<n>,std::vector<double>> cache;
	Key<n> key;

public:
	Table(){

	}
	std::vector<double>& FF(std::vector<char>& board){
		key = Key<n>(&board.front());
		auto& v = cache[key];
		if(v.size()==0){
			//optimistic outlook
			v.resize(4,1.0);
		}
		return v;
	}
	void BP(std::vector<double> Y){
		cache[key].swap(Y);
	}
};

#endif
