CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: main.cpp client.cpp solve.cpp ../socket/socket_client.cpp
	$(CXX) -o main.out main.cpp client.cpp solve.cpp asp_game_state.cpp ../socket/socket_client.cpp $(CXXFLAGS)

test: asp_game_state.cpp test.cpp
	$(CXX) -o test.out test.cpp asp_game_state.cpp $(CXXFLAGS)

run_traverser:
	./main.out 127.0.0.1 8080 0

run_adversary:
	./main.out 127.0.0.1 8080 1
