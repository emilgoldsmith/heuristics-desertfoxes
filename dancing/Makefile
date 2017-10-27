CXX = g++
CXXFLAGS = -std=c++11

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

main: main.cpp client.cpp solve.cpp
	$(CXX) -o main.out main.cpp ../socket/socket_client.cpp client.cpp solve.cpp $(CXXFLAGS)

run_emil_test: emil_test.cpp solve.cpp spiral_iterator.cpp
	$(CXX) -o emil_test.out emil_test.cpp solve.cpp spiral_iterator.cpp $(CXXFLAGS)
	./emil_test.out
