#ifndef __BOARD_H__
#define __BOARD_H__

#include <iostream>
#include <iomanip>
#include <bitset>
#include <boost/utility/binary.hpp>

#include <algorithm>

using namespace std;
using ull = unsigned long long;
enum DIR: char {X,R,U,L,D};

template<int n, int m>
class Board{
private:
	char _board[n][m] = {};
	ull _repr;
	bool _update;
public:
	Board(){
		srand(time(0));
		_repr = 0;
		_update = false;	
	};
	explicit Board(ull _repr):_repr(_repr){
		srand(time(0));
		static char mask = BOOST_BINARY(1111);
		_update = false;
		for(int i=n-1;i>=0;--i){
			for(int j=m-1;j>=0;--j){
				_board[i][j] = _repr & mask; //15 = 0b1111 
				_repr >>=4; //the local _repr, non-member
				//cout << bitset<4>(_repr & mask) << endl;
			}
		}
	}
	
	void set(int i, int j, char val){
		_board[i][j] = val;
		_update = true;
	}
	void update(){
		if(_update){
			for(auto& r : _board){
				for(auto& c : r){
					_repr <<= 4;
					_repr += c;
				}
			}
			_update = false;
		}
	}
	void randTile(){// maybe add params to how many random tiles are desired
		std::vector<int> empty;
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				if(!_board[i][j]){//empty
					empty.push_back(i*m+j);
				}
			}
		}
		//if empty.size() == 0 then gameOver()
		std::random_shuffle(empty.begin(),empty.end());
		_board[empty.front()/m][empty.front()%m] = (float(rand())/RAND_MAX < 0.1)? 2 : 1;

		_update = true;
		//get empty tiles
		//
	}

	ull repr(){
		update(); //whenever a query occurs, update if necessary
		return _repr;
	}
	void next(DIR dir){
		bool addTile = false;
		switch(dir){
			case R:
				for(int i=0;i<n;++i){
					for(int j=m-1;j>=0;--j){
						for(auto tj=j-1;tj>=0; --tj){
							if(_board[i][tj]==0)
								continue;
							else if(_board[i][j] == _board[i][tj]){
								++_board[i][j];
								_board[i][tj] = 0;
								addTile = true;
								break;
							}else{
								if(_board[i][j] == 0){
									_board[i][j] = _board[i][tj];
									_board[i][tj] = 0;
									++j;
									addTile = true;
									break;
								}
							}
						}
					}
				}
				break;
			case U:
				for(int j=0;j<m;++j){
					for(int i=0;i<n;++i){
						for(auto ti=i+1; ti<n; ++ti){
							if(_board[ti][j]==0)
								continue;
							else if(_board[i][j] == _board[ti][j]){
								++_board[i][j];
								_board[ti][j] = 0;
								addTile = true;
								break;
							}else{
								if(_board[i][j] == 0){
									_board[i][j] = _board[ti][j];
									_board[ti][j] = 0;
									addTile = true;
									--i;
									break;
								}
							}
						}
					}
				}
				break;
			case L:
				for(int i=0;i<n;++i){
					for(int j=0;j<m;++j){
						for(auto tj=j+1; tj<m; ++tj){
							if(_board[i][tj]==0)
								continue;
							else if(_board[i][j] == _board[i][tj]){
								++_board[i][j];
								_board[i][tj] = 0;
								addTile = true;
								break;
							}else{
								if(_board[i][j] == 0){
									_board[i][j] = _board[i][tj];
									_board[i][tj] = 0;
									addTile = true;
									--j;
									break;
								}
							}
						}
					}
				}
				break;
			case D:
				for(int j=0;j<m;++j){
					for(int i=n-1;i>=0;--i){
						for(auto ti=i-1; ti>=0; --ti){
							if(_board[ti][j]==0)
								continue;
							else if(_board[i][j] == _board[ti][j]){
								++_board[i][j];
								_board[ti][j] = 0;
								addTile = true;
								break;
							}else{
								if(_board[i][j] == 0){
									_board[i][j] = _board[ti][j];
									_board[ti][j] = 0;
									addTile = true;
									++i;
									break;
								}
							}
						}
					}
				}
				break;
			case X:
				//do nothing
				break;
			default:
				//do nothing
				break;
		}
		if(addTile){
			randTile();
			_update = true;
		}
	}
	void print(){
		cout << "----" << endl;
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				cout << setw(3) << (int)_board[i][j];
			}
			cout << endl;
		}
		cout << "----" << endl;
	}
};

#endif
