#pragma once
#include <vector>
// Vasikoi typoi kai entities tou paixnidiou
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
// Koini vasi gia game objects (id + thesi)
struct Entity {
    int id = 0;
    Point position;
    virtual ~Entity() = default;
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
// Enemy me path kai katastasi
struct Enemy : Entity {
    float speed = 0.0f;
    float health = 0.0f;
    float maxHealth = 0.0f;
    std::vector<int> path;
    int pathIndex = 0;
    bool isAlive = false;
    float randomWeight = 0.0f;
};
// Tower me range/fire rate/damage
struct Tower : Entity {
    int towerType = 0;
    float range = 0.0f;
    float fireRate = 0.0f;
    float damage = 0.0f;
    float timeSinceLastShot = 0.0f;
    int cost = 0;
};
// Projectile pros enemy
struct Projectile : Entity {
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
