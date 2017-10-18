#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "structs.h"

#include <vector>

class GameState {
  public:
    /* PROPERTIES */
    bool preyMoves = false;
    Position prey = {230, 200};
    Position hunter = {0, 0};
    Position hunterDirection = {1, 1};
    std::vector<Wall> walls;
    const short int cooldown;
    const short int maxWalls;
    short int cooldownTimer = 0;

    /* METHODS */
    GameState(short int cooldownVariable, short int maxWallsVariable): cooldown(cooldownVariable),
                                                                    maxWalls(maxWallsVariable)
    {}
    /**
     * The implicit copy constructor should be good enough for this game state
     * so we don't define one but can still use it.
     */
    /**
     * For positions where the prey can't move moveForPrey should be {2, 2}.
     * Also, for moveForHunter it's assumed that the client itself parses which walls
     * the hunter added / deleted
     */
    void makeMove(HunterMove moveForHunter, Position moveForPrey);
};

#endif
