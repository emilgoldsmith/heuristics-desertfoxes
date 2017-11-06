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

all: poser solver

poser: poser.cpp ../random/random.cpp
	$(CXX) -o poser.out poser.cpp ../random/random.cpp $(CXXFLAGS)

solver: solver.cpp ../random/random.cpp
	$(CXX) -o solver.out solver.cpp ../random/random.cpp $(CXXFLAGS)
