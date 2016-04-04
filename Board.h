#ifndef __BOARD_H__
#define __BOARD_H__

#include <iostream>
#include <iomanip>
#include <bitset>
#include <boost/utility/binary.hpp>
#include <string.h>

#include <algorithm>

using namespace std;
using ull = unsigned long long;
enum DIR: char {R,U,L,D,X};//X is considered out-of-bounds

template<int n, int m>
class Board{
private:
	char _board[n][m] = {};
	bool addTile;
	bool _end;

	char _next[4][n][m] = {};
	bool _nextDir[4] = {};
	int _nextR[4] = {}; //next reward

	std::vector<double> v;
public:
	Board(){
		_end = false;
		randTile(_board); //put 2 random Tiles
		randTile(_board);
		checkAvailable();
		v = toVec(_board);
	};

	void set(int i, int j, char val){
		_board[i][j] = val;
	}
	void randTile(char board[n][m]){// maybe add params to how many random tiles are desired
		std::vector<int> empty;
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				if(!board[i][j]){//empty
					empty.push_back(i*m+j);
				}
			}
		}
		std::random_shuffle(empty.begin(),empty.end());
		board[empty.front()/m][empty.front()%m] = (float(rand())/RAND_MAX < 0.1)? 2 : 1;
		//get empty tiles
		//
	}

	int ifnext(DIR dir, char board[n][m]){
		memcpy(board,_board,sizeof(_board));
		return next(dir, board);
	}
	int next(DIR dir){ //go to precalculated next state
		memcpy(_board,_next[dir],sizeof(_board));
		checkAvailable(); //calculate next available states
		v = toVec(_board);
		return _nextR[dir];
	}
	int next(DIR dir, char board[n][m]){
		int reward = 0;
		addTile = false;
		switch(dir){
			case R:
				for(int i=0;i<n;++i){
					for(int j=m-1;j>=0;--j){
						for(auto tj=j-1;tj>=0; --tj){
							if(board[i][tj]==0)
								continue;
							else if(board[i][j] == board[i][tj]){
								reward += 1<<(++board[i][j]);
								board[i][tj] = 0;
								addTile = true;
								break;
							}else{
								if(board[i][j] == 0){
									board[i][j] = board[i][tj];
									board[i][tj] = 0;
									++j;
									addTile = true;
								}
								break;
							}
						}
					}
				}
				break;
			case U:
				for(int j=0;j<m;++j){
					for(int i=0;i<n;++i){
						for(auto ti=i+1; ti<n; ++ti){
							if(board[ti][j]==0)
								continue;
							else if(board[i][j] == board[ti][j]){
								reward += 1<<(++board[i][j]);
								board[ti][j] = 0;
								addTile = true;
								break;
							}else{
								if(board[i][j] == 0){
									board[i][j] = board[ti][j];
									board[ti][j] = 0;
									addTile = true;
									--i;
								}
								break;
							}
						}
					}
				}
				break;
			case L:
				for(int i=0;i<n;++i){
					for(int j=0;j<m;++j){
						for(auto tj=j+1; tj<m; ++tj){
							if(board[i][tj]==0)
								continue;
							else if(board[i][j] == board[i][tj]){
								reward += 1<<(++board[i][j]);
								board[i][tj] = 0;
								addTile = true;
								break;
							}else{
								if(board[i][j] == 0){
									board[i][j] = board[i][tj];
									board[i][tj] = 0;
									addTile = true;
									--j;
								}
								break;
							}
						}
					}
				}
				break;
			case D:
				for(int j=0;j<m;++j){
					for(int i=n-1;i>=0;--i){
						for(auto ti=i-1; ti>=0; --ti){
							if(board[ti][j]==0)
								continue;
							else if(board[i][j] == board[ti][j]){
								reward += 1<<(++board[i][j]);
								board[ti][j] = 0;
								addTile = true;
								break;
							}else{
								if(board[i][j] == 0){
									board[i][j] = board[ti][j];
									board[ti][j] = 0;
									addTile = true;
									++i;
								}
								break;
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
			//add random tile to empty loc on board
			randTile(board);
		}
		return reward;
	}

	void print(char board[n][m]){
		cout << "----" << endl;
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				cout << setw(3) << (int)board[i][j];
			}
			cout << endl;
		}
		cout << "----" << endl;
	}
	void print(){
		print(_board);
	}

	static std::vector<double> toVec(char board[n][m]){
		std::vector<double> res((char*)board,(char*)board+n*m);
		//normalize
		double mv = *std::max_element(res.begin(),res.end());
		if(mv != 0){//would probably be true, though.
			for(auto& e : res){
				e /= mv;
			}
		}
		return res; 
	}
	std::vector<double>& vec(){
		return v;
	}
	bool isFull(){
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				if(_board[i][j] == 0)
					return false;
			}
		}
		return true;
	}

	bool end(){
		return _end;

	}
	void checkAvailable(){ //check fruitful actions
		_end = true;
		for(int dir=0;dir<4;++dir){
			// store next state to _next[dir]
			// and store reward of that to _nextR[dir]
			_nextR[dir] = ifnext((DIR)dir,_next[dir]);
			_nextDir[dir] = addTile; // = fruitful action
			
			//check Terminal
			if(_nextDir[dir])
				_end = false; //there exists a fruitful action
		}
		//std::cout << "SELF :: " << std::endl;
		//print(_board);
		//std::cout << "AVAILABLE :: " << std::endl;
		//for(int dir=0;dir<4;++dir){
		//	print(_next[dir]);
		//}
	}
	const bool* getAvailable(){
		//std::cout << "NEXTDIR : ";
		//for(int i=0;i<4;++i){
		//	std::cout << _nextDir[i] << ' ';
		//}
		//std::cout << std::endl;
		return _nextDir;
	}
	const char* board(){
		return (const char*) _board;
	}
};

#endif
