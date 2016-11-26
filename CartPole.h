/*
 * Copyright 2015 Saminda Abeyruwan (saminda@cs.miami.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * CartPole.h
 *
 *  Created on: Nov 21, 2013
 *      Author: sam
 *
 *  This is the original cart-pole problem from:
 *  http://webdocs.cs.ualberta.ca/~sutton/book/code/pole.c
 *
 *  This problem uses Euler's method to simulate the plant.
 *  The implementation regulates in a deterministic and stochastic environments.
 */

#ifndef CARTPOLE_H_
#define CARTPOLE_H_

#include "Utility.h"

#include <cmath>
#include <random>

const double GRAVITY = 9.8;
const double DT = 0.02; // time, in seconds

const double X_THRESH = 2.4;
const double T_THRESH = 24 * M_PI / 180;

struct CartPoleState{
	// observable + unobservable states
	double x, vx, ax; // horizontal
	double t, vt, at; // angular
	static Randomizer r;
	CartPoleState(){
		r(x,vx,t,vt); // randomly initialize position + velocity

		ax = 0.0;
		at = 0.0; // zero accelaration, at least.
	}
	void update(){
		x += vx * DT;
		vx += ax * DT;
		t += vt * DT;
		vt += at * DT;

		// bound velocities
		//bound(-2.4, vx, 2.4);
		//bound(-4.0 * M_PI, vt, 4.0 * M_PI);

		// normalize angle
        t = fmod(t + M_PI, 2.0 * M_PI);
        if (t < 0)
          t += 2.0 * M_PI;
		t -= M_PI;
	}
	std::vector<double> vec() const{
		//return {x,vx,ax,t,vt,at};
		return {x,vx,t,vt};
	}
};

Randomizer CartPoleState::r(-.05,.05); // randomly set state from  -.1 ~ .1

class CartPoleEnv{
	protected:
		double gravity;
		double massCart;
		double massPole;
		double totalMass;
		double length;
		double poleMassLength;
		double forceMag;
		double dt;
		double fourthirds;

		CartPoleState state;

		float previousTheta, cumulatedRotation;
		int upTime;

	public:
		CartPoleEnv() :
			gravity(GRAVITY), massCart(1.0), massPole(0.1), //
			totalMass(massPole + massCart), length(0.5/* actually half the pole's length */), //
			poleMassLength(massPole * length), forceMag(10.0), //
			dt(DT), fourthirds(4.0f / 3.0f), //
			cumulatedRotation(0), upTime(0){

			}

		virtual ~CartPoleEnv()
		{

		}

		void updateTRStep()
		{

		}

		// Profiles
		void reset()
		{
			state = CartPoleState();
			previousTheta = state.t;
			cumulatedRotation = state.t;
			upTime = 0;
		}

		void step(float a) // a = action
		{
			double force, costheta, sintheta, temp;

			force = bound(-10, a, 10); // left-right force

			costheta = cos(state.t);
			sintheta = sin(state.t);

			temp = (force + poleMassLength * state.vt * state.vt * sintheta) / totalMass;
			state.at = (gravity * sintheta - costheta * temp)
				/ (length * (fourthirds - massPole * costheta * costheta / totalMass));
			state.ax = temp - poleMassLength * state.at * costheta / totalMass;

			state.update();

			float signAngleDifference = std::atan2(std::sin(state.t- previousTheta),
					std::cos(state.t- previousTheta)); // == dtheta

			cumulatedRotation += signAngleDifference;

			++upTime;
			previousTheta = state.t;
		}

		bool terminal() const
		{

			if(!within(-X_THRESH, state.x, X_THRESH))
				return true;
			if(!within(-T_THRESH, state.t, T_THRESH))
				return true;
			//if (upTime >= (20.0/*seconds*// dt))
			//  return true;

			return false;
		}

		double r() const
		{
			if (terminal())
				return -1.0f;
			else
				return cos(state.t);
				//return 1.0f;
				// surviving!
				//return (cos(state.t) - 1.0) / 2.0;
		}

		double z() const
		{
			return 0.0f;
		}

		double up() const{
			return upTime;
		}

		const CartPoleState& s(){
			return state;
		}
};

#endif /* CARTPOLE_H_ */
