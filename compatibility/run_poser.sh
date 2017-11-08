#!/bin/sh
# make poser solver
node retrieve.js poser
./poser.out < poser.in > poser.sol
node submit.js poser
