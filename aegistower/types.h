#pragma once
#include <vector>

struct Point {
	float x, y;
};

struct Circle {
	float x, y;
	float radius;

};


struct PathNode {
	int id;
	Point position;
	std::vector<int> neighbors;

	float  gCost, hCost;
	int parentId;

};

struct Enemy{
	int id;
	Point position;
	float speed;
	float health;
	std::vector<int> path;
	int pathIndex;
	bool isAlive;

};

struct Tower {
	int id;
	Point position;
	float range;
	float fireRate;
	float damage;
	float timeSinceLastShot;
};

struct fireball {
	int id;
	Point position;
	Point target;
	float speed;
	float damage;
	bool isActive;
};