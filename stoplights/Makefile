CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -g
else
  CXXFLAGS += -O2
endif

main: parser.cpp main.cpp
	$(CXX) -o main.out main.cpp parser.cpp solve.cpp ../socket/socket_client.cpp ../timer/timer.cpp $(CXXFLAGS)

generator: test_case_generator.cpp
	$(CXX) -o generator.out test_case_generator.cpp $(CXXFLAGS)

run_default:
	./main.out 127.0.0.1 12345 300000
