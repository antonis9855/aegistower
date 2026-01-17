#pragma once
#include <vector>
enum TileType {
    SPAWN = 0,
    CASTLE = 1,
    GRASS = 2,
    ROCK = 3,
    PATH = 4
};
enum Difficulty {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};
struct Point {
    float x = 0.0f;
    float y = 0.0f;
};
struct PathNode {
    int id;
    int gridX, gridY;
    Point position;
    std::vector<int> neighbors;
    float gCost;
    float hCost;
    float weight;
    int parentId;
    float fCost() const { return gCost + hCost + weight; }
};
struct Enemy {
    int id = 0;
    Point position;
    float speed = 0.0f;
    float health = 0.0f;
    float maxHealth = 0.0f;
    std::vector<int> path;
    int pathIndex = 0;
    bool isAlive = false;
    float randomWeight = 0.0f;
};
struct Tower {
    int id = 0;
    int towerType = 0;
    Point position;
    float range = 0.0f;
    float fireRate = 0.0f;
    float damage = 0.0f;
    float timeSinceLastShot = 0.0f;
    int cost = 0;
};
struct Projectile {
    int id = 0;
    Point position;
    int targetEnemyId = -1;
    float speed = 0.0f;
    float damage = 0.0f;
    bool isActive = false;
};
struct DifficultySettings {
    float enemyHealthMult;
    float enemySpeedMult;
    float goldMult;
    int startingGold;
    int startingLives;
};
const int GRID_WIDTH = 64;
const int GRID_HEIGHT = 64;
