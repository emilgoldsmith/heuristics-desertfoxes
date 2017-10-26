#ifndef SOLVE_H
#define SOLVE_H

#include "geometry.hpp"
#include "client.h"
#include "structs.h"

#include <vector>

Point computeCenter(std::vector<Point> points);
Point computeCenterBruteforce(const std::vector<Point> &points);
Point getFurthestPoint(const Point &center, const std::vector<Point> &points, bool getSecondFurthestPoint = false);
std::vector<Point> dummyPlaceStars(Client *client);
ChoreographerMove dummyGetChoreographerMove(Client *client);

#endif
