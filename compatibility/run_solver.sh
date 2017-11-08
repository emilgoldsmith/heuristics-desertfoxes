#!/bin/sh
# make poser solver
node retrieve.js solver
./solver.out < solver.in > solver.sol
node submit.js solver
