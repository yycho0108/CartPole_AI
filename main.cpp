#include <iostream>
#include <iomanip>
#include <bitset>
#include <boost/utility/binary.hpp>

using namespace std;
using ull = unsigned long long;

enum DIR: char {R,U,L,D};

template<int n, int m>
class Board{
private:
	char _board[n][m];
	ull _repr;
	bool _update;
public:
	Board(){
		_repr = 0;
		_update = false;	
	};
	explicit Board(ull _repr):_repr(_repr){
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
	ull repr(){
		update(); //whenever a query occurs, update if necessary
		return _repr;
	}

	void next(DIR dir){
	switch(dir){
			case R:
				for(int i=0;i<n;++i){
					for(int j=m-1;j-1>=0;--j){
						for(auto tj=j; !_board[i][tj] && tj-1>=0; --tj){
							_board[i][tj] = _board[i][tj-1];
							_board[i][tj-1] = 0;
						}
						if (_board[i][j] && _board[i][j] == _board[i][j-1]){
							++_board[i][j];
							_board[i][j-1] = 0;
						}
					}
				}
				break;
			case U:
				for(int j=0;j<m;++j){
					for(int i=0;i+1<n;++i){
						for(auto ti=i; !_board[ti][j] && ti+1<n; ++ti){
							_board[ti][j] = _board[ti+1][j];
							_board[ti+1][j] = 0;
						}
						if (_board[i][j] && _board[i][j] == _board[i+1][j]){
							++_board[i][j];
							_board[i+1][j] = 0;
						}
					}
				}
				break;
			case L:
				for(int i=0;i<n;++i){
					for(int j=0;j+1<m;++j){
						for(auto tj=j; !_board[i][tj] && tj+1<m; ++tj){
							_board[i][tj] = _board[i][tj+1];
							_board[i][tj+1] = 0;
						}
						if (_board[i][j] && _board[i][j] == _board[i][j+1]){
							++_board[i][j];
							_board[i][j+1] = 0;
						}
					}
				}
				break;
			case D:
				for(int j=0;j<m;++j){
					for(int i=n-1;i-1>=0;--i){
						for(auto ti=i; !_board[ti][j] && ti-1>=0; --ti){
							_board[ti][j] = _board[ti-1][j];
							_board[ti-1][j] = 0;
						}
						if (_board[i][j] && _board[i][j] == _board[i-1][j]){
							++_board[i][j];
							_board[i-1][j] = 0;
						}
					}
				}
				break;
		}
		_update = true;
	}
	void print(){
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				cout << setw(3) << (int)_board[i][j];
			}
			cout << endl;
		}
	}
};

class Game{
	Board<4,4> board;

};

int main(){
	Board<4,4> b; //up to 16 supported by current implementation
	for(int i=0;i<4;++i){
		for(int j=0;j<4;++j){
			b.set(i,j,1);
		}
	}
	Board<4,4> c(b.repr());

	c.print();
	c.next(DIR::D);
	c.print();

	cout << "B : " << b.repr() << endl;
	cout << "C : " << c.repr() << endl;
}
