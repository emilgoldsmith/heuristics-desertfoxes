#include "structs.h"
#include "game_state.h"
#include "bresenham.h"

#include <vector>
#include <iostream>
#include <utility>

using namespace std;

GameState::GameState(int cooldownVariable, int maxWallsVariable):
  cooldown(cooldownVariable),
  maxWalls(maxWallsVariable)
{}

void GameState::makeMove(HunterMove moveForHunter, Position preyDirection) {
#ifdef DEBUG
  if (gameOver) {
    cerr << "Attempted making a move after game is over" << endl;
  }
  // Simple move validations
  if ((preyDirection.x != 2 || preyDirection.y != 2) && !preyMoves) {
    cerr << "Tried to make move for prey while not allowed" << endl;
  }
  if ((preyDirection.x > 1 || preyDirection.x < -1 || preyDirection.y > 1 || preyDirection.y < -1) && preyMoves) {
    cerr << "Prey invalid move" << endl;
  }
  if (moveForHunter.wallType != 0 && cooldownTimer > 0) {
    cerr << "Attempted to create wall while on cooldown" << endl;
  }
  for (int index : moveForHunter.indicesToDelete) {
    if (index >= walls.size() || index < 0) {
      cerr << "Attempted to remove wall with incorrect index" << endl;
    }
  }
#endif

  moveHunter(moveForHunter);
  if (preyMoves) {
    movePrey(preyDirection);
  }

  // Update general game state variables
  preyMoves = !preyMoves;
  if (cooldownTimer > 0) cooldownTimer--;
  score++;
  checkCapture();
}

void GameState::checkCapture() {
  int dx = hunter.x - prey.x;
  int dy = hunter.y - prey.y;
  int squaredDistance = dx*dx + dy*dy;
  if (squaredDistance <= 16) {
    vector<Position> pointsToConsider = bresenham(hunter, prey);
    for (Position p : pointsToConsider) {
      if (isOccupied(p)) {
        // We consider this to be a wall between, so no capture
        return;
      }
    }
    // No wall between so it is considered a capture
    gameOver = true;
  }
}

void GameState::movePrey(Position preyDirection) {
  prey = bounce(prey, preyDirection).first;
}

void GameState::moveHunter(HunterMove moveForHunter) {
  removeWalls(moveForHunter.indicesToDelete);
  // We move before creating the wall to make sure that the wall isn't being created in our current path
  Position prevHunterPosition = hunter;
  pair<Position, Position> positionAndDirection = bounce(hunter, hunterDirection);
  hunter = positionAndDirection.first;
  hunterDirection = positionAndDirection.second;
  // Now we build the wall
  buildWall(moveForHunter.wallType, prevHunterPosition);
}

void GameState::removeWalls(vector<int> indicesToDelete) {
  vector<Wall> newWalls;
  for (int i = 0; i < walls.size(); i++) {
    bool shouldDelete = false;
    for (int index : indicesToDelete) {
      if (i == index) {
        shouldDelete = true;
        break;
      }
    }
    if (!shouldDelete) {
      newWalls.push_back(walls[i]);
    }
  }
  walls = newWalls;
}

void GameState::buildWall(int wallType, Position startPosition) {
  Position start = startPosition, end = startPosition;
  if (wallType == 0) {
    // Don't do anything
    return;
  } else if (wallType == 1) {
    // Horizontal wall
    for (; !isOccupied(start); start.x--) {
      if (start == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (start == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
    }
    for (; !isOccupied(end); end.x++) {
      if (end == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (end == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
    }
    start.x++;
    end.x--;
  } else if (wallType == 2) {
    // Vertical wall
    for (; !isOccupied(start); start.y--) {
      if (start == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (start == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
    }
    for (; !isOccupied(end); end.y++) {
      if (end == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (end == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
    }
    start.y++;
    end.y--;
  } else if (wallType == 3) {
    // Diagonal wall
    bool goVertical = true;
    while(!isOccupied(start)) {
      if (start == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (start == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
      if (goVertical) {
        start.y--;
      } else {
        start.x--;
      }
      goVertical = !goVertical;
    }
    if (goVertical) {
      // We went horizontal last time
      start.x++;
    } else {
      start.y++;
    }
    goVertical = false;
    while(!isOccupied(end)) {
      if (end == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (end == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
      if (goVertical) {
        end.y++;
      } else {
        end.x++;
      }
      goVertical = !goVertical;
    }
    if (goVertical) {
      // We went horizontal last time
      start.x--;
    } else {
      start.y--;
    }
  } else if (wallType == 4) {
    // Counterdiagonal wall
    bool goVertical = false;
    while(!isOccupied(start)) {
      if (start == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (start == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
      if (goVertical) {
        start.y++;
      } else {
        start.x--;
      }
      goVertical = !goVertical;
    }
    if (goVertical) {
      // We went horizontal last time
      start.x++;
    } else {
      start.y--;
    }
    goVertical = true;
    while(!isOccupied(end)) {
      if (end == prey) {
#ifdef DEBUG
        cerr << "Wall collided with prey" << endl;
#endif
        return;
      }
      if (end == hunter) {
#ifdef DEBUG
        cerr << "Wall collided with hunter" << endl;
#endif
        return;
      }
      if (goVertical) {
        end.y--;
      } else {
        end.x++;
      }
      goVertical = !goVertical;
    }
    if (goVertical) {
      // We went horizontal last time
      end.x--;
    } else {
      end.y++;
    }
  }
  cooldownTimer = cooldown;
  walls.push_back({start, end, startPosition});
}

pair<Position, Position> GameState::bounce(Position curPosition, Position direction) {
  Position target = curPosition + direction;
  if (!isOccupied(target)) {
    return {target, direction};
  }
  if (direction.x == 0 || direction.y == 0) {
    // We are not moving diagonally
    if (direction.x != 0) {
      // We are moving horizontally so don't move, just change direction
      return {curPosition, {-direction.x, direction.y}};
    } else {
      // We are moving vertically so don't move, just change direction
      return {curPosition, {direction.x, -direction.y}};
    }
  }
  // We are moving diagonally
  bool horizontalNeighbourOccupied = isOccupied({curPosition.x + direction.x, curPosition.y});
  bool verticalNeighbourOccupied = isOccupied({curPosition.x, curPosition.y + direction.y});
  if (horizontalNeighbourOccupied && verticalNeighbourOccupied) {
    // We are enclosed in a corner
    return {curPosition, {-direction.x, -direction.y}};
  }
  if (horizontalNeighbourOccupied) {
    // It is a vertical wall
    return {{curPosition.x, target.y}, {-direction.x, direction.y}};
  }
  if (verticalNeighbourOccupied) {
    // It is a horizontal wall
    return {{target.x, curPosition.y}, {direction.x, -direction.y}};
  }
  // If none of the other direct neighbours are occupied we need to expand our search to the
  // neighbours of the target square
  bool targetHorizontalNeighbourOccupied = isOccupied({target.x + direction.x, target.y});
  bool targetVerticalNeighbourOccupied = isOccupied({target.x, target.y + direction.y});
  if (targetHorizontalNeighbourOccupied == targetVerticalNeighbourOccupied) {
    // We are at a corner one way or the other
    return {curPosition, {-direction.x, -direction.y}};
  }
  if (targetHorizontalNeighbourOccupied) {
    // It is a horizontal wall
    return {{target.x, curPosition.y}, {direction.x, -direction.y}};
  }
  if (verticalNeighbourOccupied) {
    // It is a vertical wall
    return {{curPosition.x, target.y}, {-direction.x, direction.y}};
  }
  // This should've exhausted all cases
#ifdef DEBUG
  cerr << "Reached the end of bounce function when all cases should've been exhausted" << endl;
#endif
  return {curPosition, direction};
}

bool GameState::isOccupied(Position p, int& output) {
  // We first check if we're on the edge of the board
  if (p.x < 0 || p.x >= boardSize.x || p.y < 0 || p.y >= boardSize.y) {
    output = -1;
    return true;
  }
  // Now we check for collissions with walls
  for (int index = 0; index < walls.size(); index++) {
    Wall curWall = walls[index];
    // We always order start and end so that start has lower values than end with priority on x
    if (curWall.start == curWall.end) {
      // wall is just 1 point for crazy reasons
      if (curWall.start == p) {
        output = index;
        return true;
      }
    } else if (curWall.start.x == curWall.end.x) {
      // It is vertical or 2-length diagonal which is the same
      if (p.x == curWall.start.x && p.y >= curWall.start.y && p.y <= curWall.end.y) {
        output = index;
        return true;
      }
    } else if (curWall.start.y == curWall.end.y) {
      // It is horizontal or 2-length diagonal which is the same
      if (p.y == curWall.start.y && p.x >= curWall.start.x && p.x <= curWall.end.x) {
        output = index;
        return true;
      }
    } else if (curWall.end.y > curWall.start.y) {
      // Since we know start has lower value of x we know this is a diagonal (type 3)
      int pDiagonalIndex = p.x - p.y;
      int wallDiagonalIndex1 = curWall.creationPoint.x - curWall.creationPoint.y;
      int wallDiagonalIndex2 = wallDiagonalIndex1 - 1;
      if (p.x >= curWall.start.x && p.x <= curWall.end.x && p.y >= curWall.start.y && p.y <= curWall.end.y) {
        // p is in the "square" of the diagonal
        if (pDiagonalIndex == wallDiagonalIndex1 || pDiagonalIndex == wallDiagonalIndex2) {
          // p is also on one of the diagonals
          output = index;
          return true;
        }
      }
    } else if (curWall.end.y < curWall.start.y) {
      // Since we know start has lower value of x we know this is a counterdiagonal (type 4)
      int pDiagonalIndex = p.x - p.y;
      int wallDiagonalIndex1 = curWall.creationPoint.x + curWall.creationPoint.y;
      int wallDiagonalIndex2 = wallDiagonalIndex1 - 1;
      if (p.x >= curWall.start.x && p.x <= curWall.end.x && p.y >= curWall.end.y && p.y <= curWall.start.y) {
        // p is in the "square" of the diagonal
        if (pDiagonalIndex == wallDiagonalIndex1 || pDiagonalIndex == wallDiagonalIndex2) {
          // p is also on one of the diagonals
          output = index;
          return true;
        }
      }
    }
  }
  // No wall was found that collides with the point
  output = -1;
  return false;
}
