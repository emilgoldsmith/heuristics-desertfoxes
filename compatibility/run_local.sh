#!/bin/sh
# make poser solver
node init.js
node retrieve.js poser
./poser.out < poser.in > poser.sol
node submit.js poser
node retrieve.js solver
./solver.out < solver.in > solver.sol
node submit.js solver
