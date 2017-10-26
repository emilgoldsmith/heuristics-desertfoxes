#ifndef SOLVE_H
#define SOLVE_H

#include "geometry.h"
#include "client.h"

#include <vector>

Point computeCenter(std::vector<Point> points);
Point computeCenterBruteforce(const std::vector<Point> &points);
Point getFurthestPoint(const Point &center, const std::vector<Point> &points, bool getSecondFurthestPoint = false);
std::vector<Point> dummyPlaceStars(Client *client);

#endif
