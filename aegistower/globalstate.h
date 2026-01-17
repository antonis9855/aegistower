#pragma once
#include "sgg/graphics.h"
#include <string>
#include <vector>
#include "widget.h"
#include "types.h"
class Globalstate
{
public:
    enum class GameMode
    {
        MENU,
        DIFFICULTY_SELECT,
        PLAYINGMODE,
        GAME_OVER
    };
    ~Globalstate();
    static Globalstate* getInstance();
    std::string getImagesDir() const { return m_images_path; }
    std::string getFontsDir() const { return m_fonts_path; }
    float getMouse_pos_x() const { return static_cast<float>(m_mouse.cur_pos_x); }
    float getMouse_pos_y() const { return static_cast<float>(m_mouse.cur_pos_y); }
    bool isMouseLeftPressed() const { return m_mouse.button_left_pressed; }
    void clearWidgets();
    int getGold() const { return m_gold; }
    int getLives() const { return m_lives; }
    int getCurrentWave() const { return m_currentWave; }
    int getGridWidth() const { return m_gridWidth; }
    int getGridHeight() const { return m_gridHeight; }
    const std::vector<std::vector<int>>& getMapData() const { return m_mapData; }
    const std::vector<PathNode>& getPathNodes() const { return m_pathNodes; }
    std::vector<PathNode>& getPathNodesMutable() { return m_pathNodes; }
    int getSpawnNodeId() const { return m_spawnNodeId; }
    int getCastleNodeId() const { return m_castleNodeId; }
    Point gridToScreen(int gridX, int gridY) const;
    void screenToGrid(float screenX, float screenY, int& gridX, int& gridY) const;
    bool canPlaceTower(int gridX, int gridY) const;
    void update_menu();
    void update_difficulty_select();
    void update_playingmode();
    void update_gameover();
    void draw_menu();
    void draw_difficulty_select();
    void draw_playingmode();
    void draw_gameover();
    void drawMap();
    void init();
    void draw();
    void update(float dt);
    void Run();
private:
    Globalstate();
    void setDifficulty(Difficulty diff);
    void startGame();
    void nextWave();
    void buildPathGraph();
    Point getNodePosition(int nodeId) const;
    void spawnEnemy();
    void updateEnemies(float dt);
    void placeTower(int gridX, int gridY, int towerType);
    int getTowerCost(int towerType) const;
    void updateTowers(float dt);
    void updateProjectiles(float dt);
    std::string m_images_path = "assets/";
    std::string m_fonts_path = m_images_path + "Kreon Regular.ttf";
    float m_canvas_width;
    float m_canvas_height;
    graphics::MouseState m_mouse;
    static Globalstate* m_instance;
    std::vector<Widget*> m_Allwidget;
    GameMode m_current_mode;
    Difficulty m_difficulty;
    DifficultySettings m_diffSettings;
    int m_gold;
    int m_lives;
    int m_currentWave;
    bool m_waveInProgress;
    int m_selectedTowerType;
    float m_enemySpawnTimer;
    int m_enemiesToSpawn;
    int m_nextEnemyId;
    int m_nextTowerId;
    int m_nextProjectileId;
    std::vector<std::vector<int>> m_mapData;
    std::vector<std::vector<bool>> m_towerPlaced;
    std::vector<PathNode> m_pathNodes;
    int m_gridWidth;
    int m_gridHeight;
    float m_tileSize;
    float m_mapOffsetX;
    float m_mapOffsetY;
    int m_spawnNodeId;
    int m_castleNodeId;
    std::vector<Enemy*> m_enemies;
    std::vector<Tower*> m_towers;
    std::vector<Projectile*> m_projectiles;
};
