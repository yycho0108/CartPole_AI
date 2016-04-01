#ifndef __GAMEMANGER_H__
#define __GAMEMANGER_H__
#include "Board.h"
#include "Agent.h"
#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>

enum : char {RELEASED, PRESSED, REPEATED};

std::vector<int> topology(int n, int m){
	return std::vector<int>({n*m+4,n*m/2,n*m/4,1});
}

template<int n, int m>
class GameManager{
private:
	int kb;
	Agent<n,m> ai;
	input_event ev;
	Board<n,m> board;
	std::string who;
public:
	GameManager(std::string who):who(who),ai(topology(n,m)){
		srand(time(0));
		for(auto& c : who){
			c = std::tolower(c);
		}
		if(who == "kb"){
			std::cout << "KB" << endl;
			const char* dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
			kb = open(dev,O_RDONLY); //read keyboard
			if(kb == -1){
				fprintf(stderr,"Cannot open device %s: %s.\n", dev,strerror(errno));
				close(kb);
				throw("CANNOT OPEN DEVICE");
			}
		}else if (who == "ai"){
			std::cout << "AI" << endl;
			//AI SETUP CODE HERE
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
		//confidence as param...
		dir = ai.getNext(board);
		return true;
	}
	void run(int max_epoch = 500){
		DIR dir = X;
		int score = 0;
		int epoch = 0;
		float alpha = 0.1; //= confidence
		std::vector<int> scores;
		float maxR=1.0;

		//got rid of maxR because it casts doubts
		while(CMDread(dir) && epoch < max_epoch){ //select action

			//UPDATE Q-Value
			std::vector<double> SA = board.toVec();//"previous state"
			auto s = SA.size();
			
			SA.resize(s+4);
			SA[s+(int)dir]=1.0; //action
			
			//namedPrint(SA);

			float r = board.next(dir);
			//carry out action, observe reward, new state

			maxR = r>maxR?r:maxR;
			//namedPrint(maxR);
			score += r;

			//r /= 1024.0; //normalize
			r /= 2048.0; //normalize

			//board.print();
			if(dir==X || board.end()){

				namedPrint(epoch);
				namedPrint(score);
				++epoch;

				board.print();
				//terminal state
				alpha = tanh(epoch / max_epoch);
				ai.update(SA,r,-1.0,alpha);

				//const char* b = board.board();
				//score = *std::max_element(b,b+n*m);
				scores.push_back(score);

				board = Board<n,m>();
				score = 0;
				//namedPrint(epoch);
			}else{
				//usual state
				float mv = ai.getMax(board);//max of this state given optimal policy
				// v = previous state
				// mv = max of this state given optimal policy
				// r = reward of reaching this state
				ai.update(SA,r,mv,alpha);
				//state, action, reward, maxQ(next), gamma
			}
		}


		std::ofstream f_score("scores.csv");
		for(auto& s : scores){
			f_score << s << endl;
		}
		f_score.close();

//		std::ofstream f_dir("dirs.csv");
//		for(auto& d : dirs){
//			f_dir << (int)d << endl;
//		}
//		f_dir.close();
	}
};
#endif
