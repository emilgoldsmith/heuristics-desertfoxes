CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g -O0
else
  CXXFLAGS += -O2
endif

emil_test: emil_test.cpp
	$(CXX) -o emil_test.out emil_test.cpp $(CXXFLAGS)
