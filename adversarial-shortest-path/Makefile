CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g -O0
else
  CXXFLAGS += -O2
endif

main: main.cpp client.cpp solve.cpp solve-guyu.cpp ../socket/socket_client.cpp
	$(CXX) -o main.out main.cpp client.cpp solve.cpp solve-guyu.cpp asp_game_state.cpp ../socket/socket_client.cpp $(CXXFLAGS)

test: asp_game_state.cpp test.cpp
	$(CXX) -o test.out test.cpp asp_game_state.cpp $(CXXFLAGS)

emil_test: emil_test.cpp client.cpp solve.cpp ../socket/socket_client.cpp ../timer/timer.cpp asp_game_state.cpp solve-guyu.cpp
	$(CXX) -o emil_test.out emil_test.cpp solve-guyu.cpp client.cpp solve.cpp asp_game_state.cpp ../timer/timer.cpp ../socket/socket_client.cpp $(CXXFLAGS)

generator: test_case_generator.cpp
	$(CXX) -o generator.out test_case_generator.cpp $(CXXFLAGS)

run_traverser:
	./main.out 127.0.0.1 8080 0

run_adversary:
	./main.out 127.0.0.1 8080 1

emil_generate:
	./generator.out > ../myStuff/AdversarialShortestPathGame/sample/advshort.txt
