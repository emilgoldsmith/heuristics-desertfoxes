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
	$(CXX) -o no_tipping_main.out no_tipping_main.cpp no_tipping_client.cpp solve.cpp game_state.cpp ../socket/socket_client.cpp $(CXXFLAGS)

run_default:
	./main.out

run:
	./no_tipping_main.out 127.0.0.1 12345 -f

run_random:
	python2 random_client.py 127.0.0.1:12345
