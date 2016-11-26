#ifndef __GAMEMANGER_H__
#define __GAMEMANGER_H__
#include "Utility.h"
#include "Board.h"
#include "Agent.h"
#include "Params.h"
#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cassert>

enum : char {RELEASED, PRESSED, REPEATED};

template<int n, int m>
class GameManager{
private:
	int kb;
	Agent<n,m> ai;
	input_event ev;
	Board<n,m> board;
	std::string who;

	int epoch;
public:
	GameManager(std::string who)
		:who(who){

		srand(time(0));

		for(auto& c : who){
			c = std::tolower(c);
		}

		std::cout << "KB" << endl;

		const char* dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";

		kb = open(dev,O_RDONLY); //read keyboard
		if(kb == -1 && who == "kb"){
			fprintf(stderr,"Cannot open device %s: %s.\n", dev,strerror(errno));
			close(kb);
			throw("CANNOT OPEN DEVICE");
		}
		
		board.print();
	}
	~GameManager(){
		if(who == "kb")
			close(kb);
	}


	bool CMDread(DIR& dir){
		if(who == "kb")
			return KBread(dir);
		else if (who == "ai")
			return AIread(dir);
	}
	bool KBread(DIR& dir){
		dir = X;
		ssize_t bytes = read(kb,&ev,sizeof(ev));
		
		if(bytes == (ssize_t)-1){
			if(errno == EINTR){
				return true;
			}
			else{
				return false;
			}
		}else if(bytes != sizeof(ev)){
			errno = EIO;
			return false;
		}

		if(ev.type == EV_KEY && ev.value == PRESSED){
			switch(ev.code){
				case 0x006a: //RIGHT
					dir = R;
					break;
				case 0x0067: //UP
					dir = U;
					break;
				case 0x0069: //LEFT
					dir = L;
					break;
				case 0x006c: //DOWN
					dir = D;
					break;
				default:
					break;
			}
		}
		return true;
	}
	bool AIread(DIR& dir){
		//epsilon for e-greedy
		//double eps = 1.0 - (epoch / MAX_EPOCH);
		double eps = 1.0 - tanh(2*float(epoch)/MAX_EPOCH); //somewhat arbitrary, but maybe?

		if(eps < MIN_EPS)
			eps = MIN_EPS;

		dir = ai.getNext(board,eps);

		//if (epoch < max_epoch*0.3) //arbitrary border
		//	dir = ai.getRand(board);//initial random exploration
		//else
		//	dir = ai.getNext(board, eps);

		return true;
	}
	void run(){
		DIR dir = X;
		double score = 0;

		epoch = 0;

		double alpha = ALPHA; //=learning rate
		double maxR = 256.0;

		//std::vector<DIR> dirs;
		//
		std::ofstream ftrain("train.csv");
		std::ofstream ferr("loss.csv");
		std::ofstream freward("rewards.csv");
		//got rid of maxR because it casts doubts
		int u_freq = 50;
		int n_update = 200;
		int step = 0;

		while(CMDread(dir) && epoch < MAX_EPOCH){ //select action
			//if(epoch > 0.95*max_epoch)
			//	ai.verbose() = true;
			//dirs.push_back(dir);

			//UPDATE Q-Value
			
			Board<n,m> S(board);//"previous state"
			//auto N = board.speculate(dir); //next state without random tiles
			//auto S = board.cVec();//"previous state"

			double r = board.next(dir);
			score += r;

			r = log (r+1) / 7.625;
			//r = (r - 4.12) / 288; // standardize

			freward << r << '\n';
			//r += float(board.getEmpty()) / (n*m);

			//r /= 2.0;
			//r -= 1;
			//this way, r ranges from -1 ~ 1
			
			//if(r>0)
			//	r = log(r+1) / 7.624; //7.625 = log(2048)
			//r /= maxR;
			//carry out action, observe reward, new state

			maxR = r>maxR?r:maxR;
			//namedPrint(maxR);
			

			//additional reward # empty tiles

			//r /= 1024.0; //normalize
			//r /= maxR;//2048.0; //normalize
			//if(r>0)	
			//	r = 1 - 1/r; //bigger the r, closer to 1

			//board.print();
			if(dir==X || board.end()){ //terminal state
				namedPrint(epoch);
				++epoch;

				//hline();
				//board.print();
				//namedPrint(score);
				//hline();

				// alpha = 1.0 - tanh(2*float(epoch) / MAX_EPOCH); // = learning rate
				// no alpha annealing
				
				//namedPrint(alpha);
				ai.memorize(S,dir,-1.0,board); //-1 for terminal state


				//const char* b = board.board();
				//score = *std::max_element(b,b+n*m);
				ftrain << score << '\n';
				score = 0;

				board.reset();
				//namedPrint(epoch);
			}else{
				//usual state
				// board = current state
				// v = previous state
				// mv = max of this state given optimal policy
				// r = reward of reaching this state
				ai.memorize(S,dir,r,board);
				//state, action, reward, maxQ(next), gamma
			}

			++step;
			if((step>n_update) && (step % u_freq == 0)){
				ferr << ai.learn_bundle(alpha, n_update)/(alpha*alpha) << '\n';
				ai.freeze(); // how often should I freeze?
			}

		}
		//ai.print();
		//ai.printTableSize();
		//viewing the network through 1 iteration
		//
		ai.verbose() = true;	
		std::ofstream ftest("test.csv");	
		if(true || prompt("TEST?")){
			for(int i=0;i<NUM_TEST;++i){
				board = Board<n,m>();
				while(!board.end()){
					auto DO = ai.guess(board);
					namedPrint(DO);
					DIR dir = ai.getBest(board);
					namedPrint(dir);
					board.next(dir);
				}
				ftest << (2 << board.max()) << '\n';
				board.print();
				hline();
			}
		}


		ferr.flush();
		ferr.close();

		ftrain.flush();
		ftrain.close();

		ftest.flush();
		ftest.close();
	}
};
#endif
