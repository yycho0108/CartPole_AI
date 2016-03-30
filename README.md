#2048 AI
---
## Description
Actions:
- left/right/down/up

States:
n\*m (i.e. 16 for 4X4 grid)

---
## Progress

- [x] Board Complete
- [x] Game Logic Complete (for verification)
- [x] Q-Learning Agent Implementation -- faulty
- [ ] Experience Replay
- [ ] ConvNet (if needed)
- [ ] SARSA or Off-Policy Q-Learning?
- [ ] SIGINT Handling
- [ ] Save/Load Trained Network
- [x] Debugging Premature Capping Problem
- [x] Better Determination of Terminal State (without too much overhead, but not missing either)

---
## Notes

At this point, I will implement the Agent using a Deep Neural Network
with a simple multilayer construct.
If that doesn't suffice, I will implement the Agent with a Convolutional Network.
Given that the state space is anticipated to be enormous, it is impractical to use a Q-table.

---
![Running](images/game.png)
