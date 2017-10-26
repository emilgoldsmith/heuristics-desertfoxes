CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -DLOGGING -g -O0
else
	CXXFLAGS += -O2
endif

main: main.cpp
	$(CXX) -o main.out main.cpp ../socket/socket_client.cpp $(CXXFLAGS)

run_emil_test: emil_test.cpp solve.cpp
	$(CXX) -o emil_test.out emil_test.cpp solve.cpp $(CXXFLAGS)
	./emil_test.out
