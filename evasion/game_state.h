#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "structs.h"

#include <vector>
#include <utility>

class GameState {
  public:
    /* PROPERTIES */
    bool preyMoves = false;
    Position prey = {230, 200};
    Position hunter = {0, 0};
    Position hunterDirection = {1, 1};
    std::vector<Wall> walls;
    const int cooldown;
    const int maxWalls;
    int cooldownTimer = 0;
    constexpr const static Position boardSize = {300, 300};
    bool gameOver = false;
    int score = 0;

    /* METHODS */
    GameState(int cooldownVariable, int maxWallsVariable);
    /**
     * The implicit copy constructor should be good enough for this game state
     * so we don't define one but can still use it.
     */
    /**
     * For positions where the prey can't move preyDirection should be {2, 2}.
     * Also, for moveForHunter it's assumed that the client itself parses which walls
     * the hunter added / deleted
     */
    void makeMove(HunterMove moveForHunter, Position preyDirection);

  private:
    void movePrey(Position preyDirection);
    void moveHunter(HunterMove moveForHunter);
    /**
     * Returned pair is {newPosition, newDirection}
     */
    std::pair<Position, Position> bounce(Position curPosition, Position direction);
    bool isOccupied(Position p);
    void removeWalls(std::vector<int> indicesToDelete);
    void buildWall(int wallType, Position startPosition);
    void checkCapture();
};

#endif
