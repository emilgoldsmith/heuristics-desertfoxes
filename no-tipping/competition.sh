#!/bin/bash

evaluationFunctions=( 1 2 3 )
searchDepths=( 1 2 3 )
serverCommand='php ../myStuff/No-Tipping-Game-Architecture/main.php 127.0.0.1:12345 20'
clientCommand='./no_tipping_main.out 127.0.0.1 12345'
$serverCommand &
sleep 0.5
echo "Starting Clients"
$clientCommand -f &
$clientCommand &
echo $(jobs -p)
wait
echo "Game done"

# for evalFun in "${evaluationFunctions[@]}"
# do
#   for depth in "${searchDepths[@]}"
#   do
#     echo $evalFun $depth
#   done
# done

