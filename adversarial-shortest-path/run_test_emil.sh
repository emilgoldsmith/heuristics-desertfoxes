#!/bin/bash
cd ~/code/heuristics-desertfoxes/myStuff/AdversarialShortestPathGame && python -m game &

sleep 1

cd ~/code/heuristics-desertfoxes/adversarial-shortest-path && make run_adversary &

cd ~/code/heuristics-desertfoxes/adversarial-shortest-path && make run_traverser &
