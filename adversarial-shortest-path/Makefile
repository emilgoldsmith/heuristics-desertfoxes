CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: main.cpp client.cpp solve.cpp ../socket/socket_client.cpp
	$(CXX) -o main.out main.cpp client.cpp solve.cpp ../socket/socket_client.cpp $(CXXFLAGS)

run_traverser:
	./main.out 0

run_adversary:
	./main.out 1
