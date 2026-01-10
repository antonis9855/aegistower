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
    float x, y;
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
    int id;
    Point position;
    float speed;
    float health;
    float maxHealth;
    std::vector<int> path;
    int pathIndex;
    bool isAlive;
    float randomWeight;  
};
struct Tower {
    int id;
    int towerType;
    Point position;
    float range;
    float fireRate;
    float damage;
    float timeSinceLastShot;
    int cost;
};
struct Projectile {
    int id;
    Point position;
    int targetEnemyId;
    float speed;
    float damage;
    bool isActive;
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
