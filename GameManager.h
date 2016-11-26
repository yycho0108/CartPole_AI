#ifndef __GAMEMANGER_H__
#define __GAMEMANGER_H__
#include "Utility.h"
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

float a2f(float action){
	// action = 0.0 or 1.0
	// 0 = -10, 1 = 10
	return (action - 0.5) * 20.0;
	//return (action - 1.0) * 10.0;
}

enum : char {RELEASED, PRESSED, REPEATED};

class GameManager{
private:
	int kb;
	Agent ai;
	input_event ev;
	std::string who;
	CartPoleEnv env;

	int epoch;
public:
	GameManager(std::string who)
		:who(who){

		srand(time(0));

		for(auto& c : who){
			c = std::tolower(c);
		}

		const char* dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";

		kb = open(dev,O_RDONLY); //read keyboard
		if(kb == -1 && who == "kb"){
			fprintf(stderr,"Cannot open device %s: %s.\n", dev,strerror(errno));
			close(kb);
			throw("CANNOT OPEN DEVICE");
		}
	}
	~GameManager(){
		if(who == "kb")
			close(kb);
	}


	bool CMDread(float& action){
		if(who == "kb")
			return KBread(action);
		else if (who == "ai")
			return AIread(action);

		return false;
	}

	bool KBread(float& action){
		action = 0.0;
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
					action = 1.0;
					break;
				case 0x0069: //LEFT
					action = -1.0;
					break;
				default:
					break;
			}
		}
		return true;
	}

	bool AIread(float& action){
		//epsilon for e-greedy
		double eps = 1.0 - (float(epoch) / MAX_EPOCH);
		//double eps = 1.0 - tanh(2*float(epoch)/MAX_EPOCH); //somewhat arbitrary, but maybe?

		action = ai.getNext(env.s(),eps);

		//if (epoch < max_epoch*0.3) //arbitrary border
		//	action = ai.getRand(board);//initial random exploration
		//else
		//	action = ai.getNext(board, eps);

		return true;
	}
	void run(){
		float action = 0.0;
		double score = 0;

		epoch = 0;

		double alpha = ALPHA; //=learning rate

		std::ofstream ftrain("train.csv");
		std::ofstream ferr("loss.csv");
		std::ofstream fup("up.csv");

		int step = 0;

		while(epoch < MAX_EPOCH){ //select action

			score = 0;
			env.reset();
			CartPoleState prev = env.s();

			while(!env.terminal()){
				CMDread(action);
				env.step(a2f(action));
				CartPoleState s = env.s();
				double r = env.r();

				//freward << r << '\n';
				ai.memorize(prev,action,r,s); //-1 for terminal state
				prev = s; // update prev state

				score += r;

				// Train
				++step;
				if((step > U_START) && (step % U_FREQ == 0)){
					ferr << ai.learn_bundle(alpha, U_SIZE) << '\n';
					ai.freeze(); // how often should I freeze?
				}
			}
			namedPrint(epoch);
			++epoch;

			ftrain << score << '\n';
			fup << env.up() << '\n';
		}

		if(true || prompt("TEST?")){
			for(int i=0;i<NUM_TEST;++i){
				std::cout << "TEST " << i << std::endl;
				env.reset();
				while(!env.terminal()){
					auto s = env.s();
					auto DO = ai.guess(s);
					namedPrint(DO);
					auto best = ai.getBest(s);
					action = best.second;
					//namedPrint(action);
					env.step(a2f(action));
				}
			}
		}


		ferr.flush();
		ferr.close();

		ftrain.flush();
		ftrain.close();
	}
};
#endif
