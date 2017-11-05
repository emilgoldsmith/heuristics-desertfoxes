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

poser: poser.cpp ../random/random.cpp
	$(CXX) -o poser.out poser.cpp ../random/random.cpp $(CXXFLAGS)

