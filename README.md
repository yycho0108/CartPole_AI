#2048 AI
---
## Description
Actions:
- left/right/down/up

States:
n\*m (i.e. 16 for 4X4 grid)

Parameters:
- Learning Rate for Network
- Learning Rate for SARSA (alpha)
- Discount Rate (gamma)
- Network Topology (# of Neurons and Hidden Layers)
- max # of epoch
- Frequency of Random Selection (epsilon)
---
## Progress

- [x] Board Complete
- [x] Game Logic Complete (for verification)
- [x] Q-Learning Agent Implementation -- faulty
- [x] Experience Replay
- [ ] ConvNet (if needed)
- [x] SARSA or Off-Policy Q-Learning? -- SARSA
- [ ] SIGINT Handling
- [ ] Save/Load Trained Network
- [x] Debugging Premature Capping Problem
- [x] Better Determination of Terminal State
- [x] ~~Replace Deterministic Max Q-Value Exploration with Probabilistic Exploration~~ -- Unnecessary
- [x] Debug Neural Network : Back Propagation Doesn't seem to occur effectively.
	- Neural Network seems to be doing fine, simply a numerical instability for small numbers.
- ~~[x] Fix Game Logic Bug : Jumping Across Blocks~~
	- Was Running Old Code
- [x] Converting vectors into templates (since they are fixed-size)

---
## Notes

At this point, I will implement the Agent using a Deep Neural Network
with a simple multilayer construct.
If that doesn't suffice, I will implement the Agent with a Convolutional Network.
Given that the state space is anticipated to be enormous, it is impractical to use a Q-table.

---
![Running](images/game.png)

Currently, does not seem to be learning;
Scores over time on 3x3 Grid with 50000 iterations:

![Scores](images/3x3_50000.png)
