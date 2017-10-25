CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g -O0
else
	CXXFLAGS += -O2
endif

GUYU_SERVER_PATH = ../../Game\ 6\ -\ Evasion

run_emil_test: emil_test.cpp solve.cpp
	$(CXX) -o emil_test.out emil_test.cpp solve.cpp $(CXXFLAGS)
	./emil_test.out
