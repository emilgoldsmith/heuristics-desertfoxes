CXX = g++
CXXFLAGS = -std=c++11

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g -O0
else
	CXXFLAGS += -O2
endif

GUYU_SERVER_PATH = ../../Game\ 6\ -\ Evasion

emil_test: emil_test.cpp game_state.cpp bresenham.cpp evasion_solve.cpp
	$(CXX) -o emil_test.out emil_test.cpp game_state.cpp bresenham.cpp evasion_solve.cpp $(CXXFLAGS)

client_test: evasion_client.cpp
	(echo STARTING SERVER; java -jar $(GUYU_SERVER_PATH)/./target/evasion-1.0-SNAPSHOT.jar 9000 9001 100 10 127.0.0.1 8999) & (sleep 2; echo STARTING DISPLAY; node $(GUYU_SERVER_PATH)/web.js 8999 8998) & (sleep 3; echo CONNECTING PLAYER 1; python2 $(GUYU_SERVER_PATH)/./players/random_player.py 9000) & (sleep 4; echo CONNECTING PLAYER 2; python2 $(GUYU_SERVER_PATH)/./players/random_player.py 9001)

main: main.cpp evasion_client.cpp evasion_solve.cpp
	$(CXX) -o main.out main.cpp evasion_client.cpp evasion_solve.cpp game_state.cpp ../socket/socket_client.cpp bresenham.cpp $(CXXFLAGS)
