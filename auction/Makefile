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

main: main.cpp auction_client.cpp
	$(CXX) -o main.out auction_client.cpp ../socket/socket_client.cpp $(CXXFLAGS)
