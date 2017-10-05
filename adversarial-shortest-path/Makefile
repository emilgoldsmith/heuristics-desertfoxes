CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: client.cpp
	$(CXX) -o main.out client.cpp ../socket/socket_client.cpp $(CXXFLAGS)

run_default:
	./main.out

