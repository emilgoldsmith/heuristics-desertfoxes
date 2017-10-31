#ifndef SOLVE_H
#define SOLVE_H

#include "geometry.hpp"
#include "client.h"
#include "structs.h"
#include "../timer/timer.h"
#include "game_state.h"

#include <vector>

Point computeCenter(std::vector<Point> points);
Point computeCenterBruteforce(const std::vector<Point> &points);
Point getFurthestPoint(const Point &center, const std::vector<Point> &points, bool getSecondFurthestPoint = false);
std::vector<Pairing> getPairing(Client *client);
SolutionSpec pairingsToPositions(Client *client, std::vector<Pairing> pairings);
SolutionSpec solveManyPoints(Client *client, const std::vector<Dancer> &dancers, const std::vector<Point> &takenPositions);
std::vector<PointScore> getAdjCandidates(GameState &state);
std::vector<Point> adjPlaceStars(Client *client, Timer &t);
std::vector<Point> choreoPlaceStars(Client *client, Timer &t);
std::vector<Point> dummyPlaceStars(Client *client);
ChoreographerMove dummyGetChoreographerMove(Client *client);

#endif
