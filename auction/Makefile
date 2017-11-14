CXX = g++
CXXFLAGS = -std=c++14

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g -O0
else
	CXXFLAGS += -O2
endif

LOGGING ?= 0
ifeq ($(LOGGING), 1)
	CXXFLAGS += -DLOGGING
endif

VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
	CXXFLAGS += -DVERBOSE
endif

main: main.cpp auction_client.cpp solver.cpp ../timer/timer.cpp
	$(CXX) -o main.out main.cpp auction_client.cpp solver.cpp ../socket/socket_client.cpp $(CXXFLAGS) ../timer/timer.cpp

run_emil_test: emil_test.cpp solver.cpp ../timer/timer.cpp
	$(CXX) -o emil_test.out emil_test.cpp solver.cpp $(CXXFLAGS) ../timer/timer.cpp
	./emil_test.out
