CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: main.cpp game_state.cpp
	$(CXX) -o main.out main.cpp game_state.cpp $(CXXFLAGS)

run_default:
	./main.out
