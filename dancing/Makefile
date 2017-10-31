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

main: main.cpp client.cpp solve.cpp game_state.cpp spiral_iterator.cpp pairing_iterator.cpp ../random/random.cpp ../timer/timer.cpp
	$(CXX) -o main.out main.cpp ../socket/socket_client.cpp client.cpp solve.cpp spiral_iterator.cpp game_state.cpp pairing_iterator.cpp $(CXXFLAGS) ../random/random.cpp ../timer/timer.cpp

run_emil_test: emil_test.cpp solve.cpp spiral_iterator.cpp client.cpp pairing_iterator.cpp
	$(CXX) -o emil_test.out emil_test.cpp solve.cpp spiral_iterator.cpp client.cpp ../socket/socket_client.cpp pairing_iterator.cpp $(CXXFLAGS) -DTEST ../random/random.cpp
	./emil_test.out

run_guyu_test: guyu_test.cpp game_state.cpp
	$(CXX) -o guyu_test.out guyu_test.cpp game_state.cpp ../random/random.cpp $(CXXFLAGS)
	./guyu_test.out

generate:
	python3 generate_test_case.py 50 50 8 > random_test_case.txt
