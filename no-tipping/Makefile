CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: main.cpp game_state.cpp python_communicator.cpp
	$(CXX) -o main.out main.cpp game_state.cpp python_communicator.cpp $(CXXFLAGS)

ntmain: no_tipping_main.cpp no_tipping_client.cpp game_state.cpp
	$(CXX) -o no_tipping_main.out no_tipping_main.cpp no_tipping_client.cpp solve.cpp game_state.cpp ../timer/timer.cpp ../socket/socket_client.cpp $(CXXFLAGS)

vs: vs.cpp game_state.cpp solve.cpp
	$(CXX) -o vs.out vs.cpp game_state.cpp solve.cpp $(CXXFLAGS)

one_game: vs_one_game.cpp solve.cpp
	$(CXX) -o vs_one_game.out vs_one_game.cpp solve.cpp game_state.cpp ../timer/timer.cpp $(CXXFLAGS)

run_default:
	./main.out

run_first:
	./no_tipping_main.out 172.16.27.252 9000 -f

run_second:
	./no_tipping_main.out 172.16.27.252 9000

run_first8:
	./no_tipping_main.out 172.16.27.252 8000 -f

run_second8:
	./no_tipping_main.out 172.16.27.252 8000

run_random:
	python2 random_client.py 127.0.0.1:12345
