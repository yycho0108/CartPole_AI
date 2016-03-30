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
	return std::vector<int>({n*m+4,n*m/2,1});
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
	void run(){
		DIR dir = X;
		int score = 0;
		int epoch = 0;
		std::vector<int> scores;
		float maxR=1.0;
//		std::vector<DIR> dirs;

		//got rid of maxR because it casts doubts
		while(CMDread(dir)){
//			dirs.push_back(dir);
			//namedPrint(dir);
			//UPDATE Q-Value
			float r = board.next(dir);
			maxR = r>maxR?r:maxR;
			namedPrint(maxR);
			score += r;

			r /= 2048.0; //normalize

			//board.print();
			if(dir==X || board.end()){
				//board.print();
				//terminal state
				ai.update(dir,r,-1.0,0.3);
				board = Board<n,m>();
				//namedPrint(epoch);
				namedPrint(score);
				scores.push_back(score);
				score = 0;
				++epoch;
			}else{
				//usual state
				float mv = ai.getMax(board);//max of "next" state(= this state now)
				//namedPrint(mv);
				ai.update(dir,r,mv,0.3);
			}
		}


		//std::ofstream f_score("scores.csv");
		//for(auto& s : scores){
		//	f_score << s << endl;
		//}
		//f_score.close();

//		std::ofstream f_dir("dirs.csv");
//		for(auto& d : dirs){
//			f_dir << (int)d << endl;
//		}
//		f_dir.close();
	}
};
#endif
