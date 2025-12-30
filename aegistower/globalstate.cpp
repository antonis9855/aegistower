#include "globalstate.h"
#include "sgg/graphics.h"
#include "display.h"
#include "button.h"
#include "astar.h"
#include "collisions.h"
#include <cstdlib>
#include <ctime>
Globalstate* Globalstate::m_instance = nullptr;
Globalstate::Globalstate()
    : m_canvas_width(1920.0f), m_canvas_height(1080.0f),
      m_current_mode(GameMode::MENU), m_difficulty(MEDIUM),
      m_gold(0), m_lives(0), m_currentWave(0), m_waveInProgress(false),
      m_selectedTowerType(-1), m_enemySpawnTimer(0.0f),
      m_enemiesToSpawn(0), m_nextEnemyId(0), m_nextTowerId(0), m_nextProjectileId(0),
      m_gridWidth(0), m_gridHeight(0), m_tileSize(0.0f), m_mapOffsetX(0), m_mapOffsetY(0),
      m_spawnNodeId(-1), m_castleNodeId(-1)
{
    m_diffSettings = {1.0f, 1.0f, 1.0f, 200, 20};
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}
Globalstate::~Globalstate()
{
    clearWidgets();
    for (auto* e : m_enemies) delete e;
    for (auto* t : m_towers) delete t;
    for (auto* p : m_projectiles) delete p;
}
Globalstate* Globalstate::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new Globalstate();
    return m_instance;
}
void Globalstate::clearWidgets()
{
    for (Widget* widget : m_Allwidget)
        delete widget;
    m_Allwidget.clear();
}
Point Globalstate::gridToScreen(int gridX, int gridY) const
{
    Point p;
    p.x = m_mapOffsetX + gridX * m_tileSize + m_tileSize / 2.0f;
    p.y = m_mapOffsetY + gridY * m_tileSize + m_tileSize / 2.0f;
    return p;
}
void Globalstate::screenToGrid(float screenX, float screenY, int& gridX, int& gridY) const
{
    gridX = static_cast<int>((screenX - m_mapOffsetX) / m_tileSize);
    gridY = static_cast<int>((screenY - m_mapOffsetY) / m_tileSize);
}
bool Globalstate::canPlaceTower(int gridX, int gridY) const
{
    if (gridX < 0 || gridX >= m_gridWidth || gridY < 0 || gridY >= m_gridHeight)
        return false;
    return m_mapData[gridY][gridX] == GRASS && !m_towerPlaced[gridY][gridX];
}
void Globalstate::buildPathGraph()
{
    m_pathNodes.clear();
    int nodeId = 0;
    for (int y = 0; y < m_gridHeight; y++) {
        for (int x = 0; x < m_gridWidth; x++) {
            int type = m_mapData[y][x];
            if (type == PATH || type == SPAWN || type == CASTLE) {
                PathNode node;
                node.id = nodeId++;
                node.gridX = x;
                node.gridY = y;
                node.position = gridToScreen(x, y);
                node.gCost = 0;
                node.hCost = 0;
                node.weight = 0;
                node.parentId = -1;
                if (type == SPAWN) m_spawnNodeId = node.id;
                if (type == CASTLE) m_castleNodeId = node.id;
                m_pathNodes.push_back(node);
            }
        }
    }
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    for (auto& node : m_pathNodes) {
        for (int d = 0; d < 4; d++) {
            int nx = node.gridX + dx[d];
            int ny = node.gridY + dy[d];
            for (const auto& other : m_pathNodes) {
                if (other.gridX == nx && other.gridY == ny) {
                    node.neighbors.push_back(other.id);
                    break;
                }
            }
        }
    }
}
void Globalstate::setDifficulty(Difficulty diff)
{
    m_difficulty = diff;
    switch (diff) {
    case EASY:   m_diffSettings = {0.7f, 0.8f, 1.2f, 300, 30}; break;
    case MEDIUM: m_diffSettings = {1.0f, 1.0f, 1.0f, 200, 20}; break;
    case HARD:   m_diffSettings = {1.5f, 1.2f, 0.8f, 150, 10}; break;
    }
}
void Globalstate::startGame()
{
    m_gold = m_diffSettings.startingGold;
    m_lives = m_diffSettings.startingLives;
    m_currentWave = 0;
    m_waveInProgress = false;
    m_selectedTowerType = -1;
    m_nextEnemyId = 0;
    m_nextTowerId = 0;
    m_nextProjectileId = 0;
    for (auto* e : m_enemies) delete e;
    for (auto* t : m_towers) delete t;
    for (auto* p : m_projectiles) delete p;
    m_enemies.clear();
    m_towers.clear();
    m_projectiles.clear();
    m_gridWidth = GRID_WIDTH;   
    m_gridHeight = GRID_HEIGHT; 
    m_mapData.clear();
    m_mapData.resize(m_gridHeight, std::vector<int>(m_gridWidth, GRASS));
    float playAreaHeight = m_canvas_height - 200.0f;
    float playAreaWidth = m_canvas_width;
    float tileSizeByWidth = playAreaWidth / m_gridWidth;
    float tileSizeByHeight = playAreaHeight / m_gridHeight;
    m_tileSize = (tileSizeByWidth < tileSizeByHeight) ? tileSizeByWidth : tileSizeByHeight;
    float mapWidth = m_gridWidth * m_tileSize;
    float mapHeight = m_gridHeight * m_tileSize;
    m_mapOffsetX = (m_canvas_width - mapWidth) / 2.0f;
    m_mapOffsetY = (playAreaHeight - mapHeight) / 2.0f;
    int spawnY = m_gridHeight / 2;  
    m_mapData[spawnY][0] = SPAWN;
    m_mapData[spawnY][m_gridWidth - 1] = CASTLE;
    for (int x = 0; x < m_gridWidth; x++) {
        m_mapData[spawnY][x] = PATH;
    }
    m_mapData[spawnY][0] = SPAWN;  
    m_mapData[spawnY][m_gridWidth - 1] = CASTLE;  
    int topY = 10;
    for (int x = 1; x < m_gridWidth - 1; x++) {
        m_mapData[topY][x] = PATH;
    }
    int bottomY = m_gridHeight - 11;
    for (int x = 1; x < m_gridWidth - 1; x++) {
        m_mapData[bottomY][x] = PATH;
    }
    for (int y = topY; y <= bottomY; y++) {
        m_mapData[y][1] = PATH;
    }
    for (int y = topY; y <= bottomY; y++) {
        m_mapData[y][m_gridWidth - 2] = PATH;
    }
    int midX1 = 20;
    int midX2 = 44;
    for (int y = topY; y <= bottomY; y++) {
        m_mapData[y][midX1] = PATH;
        m_mapData[y][midX2] = PATH;
    }
    for (int y = 14; y < 20; y++) {
        for (int x = 8; x < 16; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 14; y < 20; y++) {
        for (int x = 48; x < 56; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 44; y < 50; y++) {
        for (int x = 8; x < 16; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 44; y < 50; y++) {
        for (int x = 48; x < 56; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 26; y < 38; y++) {
        for (int x = 26; x < 38; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    m_towerPlaced.clear();
    m_towerPlaced.resize(m_gridHeight, std::vector<bool>(m_gridWidth, false));
    buildPathGraph();
}
void Globalstate::nextWave()
{
    m_currentWave++;
    m_waveInProgress = true;
    m_enemiesToSpawn = 5 + m_currentWave * 3;
    m_enemySpawnTimer = 0.0f;
    Astar::updateNodeWeights(m_pathNodes, m_towers);
}
void Globalstate::spawnEnemy()
{
    Enemy* enemy = new Enemy();
    enemy->id = m_nextEnemyId++;
    enemy->isAlive = true;
    enemy->pathIndex = 0;
    enemy->maxHealth = (50.0f + m_currentWave * 20.0f) * m_diffSettings.enemyHealthMult;
    enemy->health = enemy->maxHealth;
    enemy->speed = (100.0f + m_currentWave * 5.0f) * m_diffSettings.enemySpeedMult;
    enemy->randomWeight = static_cast<float>(std::rand() % 50) / 10.0f;
    enemy->path = Astar::findPath(m_spawnNodeId, m_castleNodeId, m_pathNodes, enemy->randomWeight);
    if (!enemy->path.empty()) {
        for (const auto& node : m_pathNodes) {
            if (node.id == enemy->path[0]) {
                enemy->position = node.position;
                break;
            }
        }
    }
    m_enemies.push_back(enemy);
}
void Globalstate::updateEnemies(float dt)
{
    for (auto* enemy : m_enemies) {
        if (!enemy->isAlive) continue;
        if (enemy->pathIndex < static_cast<int>(enemy->path.size())) {
            int targetNodeId = enemy->path[enemy->pathIndex];
            Point targetPos = {0, 0};
            for (const auto& node : m_pathNodes) {
                if (node.id == targetNodeId) {
                    targetPos = node.position;
                    break;
                }
            }
            float dx = targetPos.x - enemy->position.x;
            float dy = targetPos.y - enemy->position.y;
            float dist = Collisions::distance(enemy->position, targetPos);
            if (dist < 5.0f) {
                enemy->pathIndex++;
            } else {
                float moveSpeed = enemy->speed * dt;
                enemy->position.x += (dx / dist) * moveSpeed;
                enemy->position.y += (dy / dist) * moveSpeed;
            }
        } else {
            enemy->isAlive = false;
            m_lives--;
            if (m_lives <= 0) {
                m_current_mode = GameMode::GAME_OVER;
            }
        }
    }
    for (auto it = m_enemies.begin(); it != m_enemies.end(); ) {
        if (!(*it)->isAlive) {
            delete *it;
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }
}
int Globalstate::getTowerCost(int towerType) const
{
    switch (towerType) {
    case 0: return 100;
    case 1: return 300;
    case 2: return 800;
    case 3: return 2000;
    default: return 100;
    }
}
void Globalstate::placeTower(int gridX, int gridY, int towerType)
{
    if (!canPlaceTower(gridX, gridY)) return;
    int cost = getTowerCost(towerType);
    if (m_gold < cost) return;
    m_gold -= cost;
    Tower* tower = new Tower();
    tower->id = m_nextTowerId++;
    tower->towerType = towerType;
    tower->position = gridToScreen(gridX, gridY);
    tower->timeSinceLastShot = 0.0f;
    tower->cost = cost;
    switch (towerType) {
    case 0: tower->range = m_tileSize * 4;  tower->fireRate = 0.5f; tower->damage = 15.0f; break;
    case 1: tower->range = m_tileSize * 5;  tower->fireRate = 0.8f; tower->damage = 35.0f; break;
    case 2: tower->range = m_tileSize * 6;  tower->fireRate = 1.2f; tower->damage = 80.0f; break;
    case 3: tower->range = m_tileSize * 8;  tower->fireRate = 0.6f; tower->damage = 120.0f; break;
    }
    m_towerPlaced[gridY][gridX] = true;
    m_towers.push_back(tower);
}
void Globalstate::updateTowers(float dt)
{
    for (auto* tower : m_towers) {
        tower->timeSinceLastShot += dt;
        if (tower->timeSinceLastShot >= tower->fireRate) {
            Enemy* target = nullptr;
            float closestDist = tower->range;
            for (auto* enemy : m_enemies) {
                if (!enemy->isAlive) continue;
                float dist = Collisions::distance(tower->position, enemy->position);
                if (dist < closestDist) {
                    closestDist = dist;
                    target = enemy;
                }
            }
            if (target) {
                Projectile* proj = new Projectile();
                proj->id = m_nextProjectileId++;
                proj->position = tower->position;
                proj->targetEnemyId = target->id;
                proj->speed = 400.0f;
                proj->damage = tower->damage;
                proj->isActive = true;
                m_projectiles.push_back(proj);
                tower->timeSinceLastShot = 0.0f;
            }
        }
    }
}
void Globalstate::updateProjectiles(float dt)
{
    for (auto* proj : m_projectiles) {
        if (!proj->isActive) continue;
        Enemy* target = nullptr;
        for (auto* enemy : m_enemies) {
            if (enemy->id == proj->targetEnemyId && enemy->isAlive) {
                target = enemy;
                break;
            }
        }
        if (!target) {
            proj->isActive = false;
            continue;
        }
        float dx = target->position.x - proj->position.x;
        float dy = target->position.y - proj->position.y;
        float dist = Collisions::distance(proj->position, target->position);
        if (dist < 15.0f) {
            target->health -= proj->damage;
            proj->isActive = false;
            if (target->health <= 0) {
                target->isAlive = false;
                m_gold += static_cast<int>(10 * m_diffSettings.goldMult);
            }
        } else {
            float moveSpeed = proj->speed * dt;
            proj->position.x += (dx / dist) * moveSpeed;
            proj->position.y += (dy / dist) * moveSpeed;
        }
    }
    for (auto it = m_projectiles.begin(); it != m_projectiles.end(); ) {
        if (!(*it)->isActive) {
            delete *it;
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
}
void Globalstate::drawMap()
{
    graphics::Brush brush;
    brush.outline_opacity = 0.0f;
    for (int y = 0; y < m_gridHeight; y++) {
        for (int x = 0; x < m_gridWidth; x++) {
            Point pos = gridToScreen(x, y);
            switch (m_mapData[y][x]) {
            case SPAWN:
                brush.fill_color[0] = 0.9f; brush.fill_color[1] = 0.2f; brush.fill_color[2] = 0.2f;
                brush.texture = m_images_path + "tiles/spawn.png";
                break;
            case CASTLE:
                brush.fill_color[0] = 0.2f; brush.fill_color[1] = 0.3f; brush.fill_color[2] = 0.9f;
                brush.texture = m_images_path + "tiles/castle.png";
                break;
            case GRASS:
                brush.fill_color[0] = 0.2f; brush.fill_color[1] = 0.6f; brush.fill_color[2] = 0.2f;
                brush.texture = m_images_path + "tiles/grass.png";
                break;
            case ROCK:
                brush.fill_color[0] = 0.4f; brush.fill_color[1] = 0.4f; brush.fill_color[2] = 0.4f;
                brush.texture = m_images_path + "tiles/rock.png";
                break;
            case PATH:
                brush.fill_color[0] = 0.76f; brush.fill_color[1] = 0.6f; brush.fill_color[2] = 0.42f;
                brush.texture = m_images_path + "tiles/path.png";
                break;
            }
            graphics::drawRect(pos.x, pos.y, m_tileSize, m_tileSize, brush);
        }
    }
}
void Globalstate::update_menu()
{
    m_Allwidget.push_back(new ResourceDisplay("menu.png", false, 0.0f, 0.0f, 1920.0f, 1080.0f));
    m_Allwidget.push_back(new Button(Button::PLAY, 780.0f, 400.0f, "play_button.png", "PLAY", 0, 360.0f, 100.0f, Widget::CENTER, Widget::CENTER, 40.0f));
    m_Allwidget.push_back(new Button(Button::QUIT, 780.0f, 520.0f, "quit_button.png", "QUIT", 0, 360.0f, 100.0f, Widget::CENTER, Widget::CENTER, 40.0f));
}
void Globalstate::draw_menu()
{
    for (Widget* widget : m_Allwidget) widget->draw();
}
void Globalstate::update_difficulty_select()
{
    m_Allwidget.push_back(new ResourceDisplay("menu.png", false, 0.0f, 0.0f, 1920.0f, 1080.0f));
    m_Allwidget.push_back(new Button(Button::BACK, 780.0f, 200.0f, "", "SELECT DIFFICULTY", 0, 360.0f, 60.0f, Widget::CENTER, Widget::CENTER, 45.0f, 50.0f, 50.0f, 50.0f, false));
    m_Allwidget.push_back(new Button(Button::DIFFICULTY_EASY, 780.0f, 350.0f, "", "EASY", 0, 360.0f, 80.0f, Widget::CENTER, Widget::CENTER, 35.0f, 100.0f, 200.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::DIFFICULTY_MEDIUM, 780.0f, 450.0f, "", "MEDIUM", 0, 360.0f, 80.0f, Widget::CENTER, Widget::CENTER, 35.0f, 200.0f, 200.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::DIFFICULTY_HARD, 780.0f, 550.0f, "", "HARD", 0, 360.0f, 80.0f, Widget::CENTER, Widget::CENTER, 35.0f, 200.0f, 100.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::BACK, 780.0f, 680.0f, "", "BACK", 0, 200.0f, 60.0f, Widget::CENTER, Widget::CENTER, 30.0f));
}
void Globalstate::draw_difficulty_select()
{
    for (Widget* widget : m_Allwidget) widget->draw();
}
void Globalstate::update_playingmode()
{
    m_Allwidget.push_back(new ResourceDisplay("", false, 0.0f, 880.0f, 1920.0f, 200.0f));
    float start_x = 100.0f;
    float spacing = 130.0f;
    float y = 920.0f;
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x, y, "BlueTower.png", "100", 100, 100.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + spacing, y, "Purple_tower.png", "300", 300, 100.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 2*spacing, y, "RedTower.png", "800", 800, 100.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 3*spacing, y, "GoldenTower.png", "2000", 2000, 100.0f, 100.0f));
    m_Allwidget.push_back(new Button(Button::START_WAVE, start_x + 4*spacing + 50.0f, y, "play_button.png", "START", 0, 100.0f, 100.0f, Widget::CENTER, Widget::BOTTOM, 20.0f));
    m_Allwidget.push_back(new Button(Button::HEALTH, 1350.0f, y, "Castle.png", std::to_string(m_lives), 0, 80.0f, 80.0f, Widget::RIGHT, Widget::CENTER, 35.0f, 194.0f, 151.0f, 112.0f, false));
    m_Allwidget.push_back(new Button(Button::INVENTORY, 1550.0f, y, "coins.png", std::to_string(m_gold), 0, 80.0f, 80.0f, Widget::RIGHT, Widget::CENTER, 35.0f, 194.0f, 151.0f, 112.0f, false));
    m_Allwidget.push_back(new Button(Button::PAUSE, 1750.0f, y, "", "Wave " + std::to_string(m_currentWave), 0, 120.0f, 50.0f, Widget::CENTER, Widget::CENTER, 25.0f, 194.0f, 151.0f, 112.0f, false));
}
void Globalstate::draw_playingmode()
{
    drawMap();
    if (m_selectedTowerType >= 0 && getMouse_pos_y() < 880.0f) {
        graphics::Brush rangeBrush;
        rangeBrush.fill_opacity = 0.2f;
        rangeBrush.fill_color[0] = 0.0f;
        rangeBrush.fill_color[1] = 1.0f;
        rangeBrush.fill_color[2] = 0.0f;
        rangeBrush.outline_opacity = 0.5f;
        rangeBrush.outline_color[0] = 0.0f;
        rangeBrush.outline_color[1] = 1.0f;
        rangeBrush.outline_color[2] = 0.0f;
        float ranges[] = {m_tileSize * 4, m_tileSize * 5, m_tileSize * 6, m_tileSize * 8};
        graphics::drawDisk(getMouse_pos_x(), getMouse_pos_y(), ranges[m_selectedTowerType], rangeBrush);
    }
    float towerSize = m_tileSize * 0.9f;
    graphics::Brush towerBrush;
    towerBrush.fill_opacity = 1.0f;
    std::string textures[] = {"BlueTower.png", "Purple_tower.png", "RedTower.png", "GoldenTower.png"};
    for (const auto* tower : m_towers) {
        towerBrush.texture = m_images_path + textures[tower->towerType];
        graphics::drawRect(tower->position.x, tower->position.y, towerSize, towerSize, towerBrush);
    }
    float enemySize = m_tileSize * 0.7f;
    graphics::Brush enemyBrush;
    enemyBrush.fill_opacity = 1.0f;
    enemyBrush.texture = m_images_path + "enemies/goblin.png";
    for (const auto* enemy : m_enemies) {
        if (!enemy->isAlive) continue;
        graphics::drawRect(enemy->position.x, enemy->position.y, enemySize, enemySize, enemyBrush);
        float hpBarWidth = m_tileSize * 0.8f;
        graphics::Brush hpBg; hpBg.fill_color[0] = 0.3f; hpBg.fill_color[1] = 0.3f; hpBg.fill_color[2] = 0.3f;
        graphics::drawRect(enemy->position.x, enemy->position.y - enemySize * 0.7f, hpBarWidth + 2, 4.0f, hpBg);
        graphics::Brush hp;
        float pct = enemy->health / enemy->maxHealth;
        hp.fill_color[0] = 1.0f - pct; hp.fill_color[1] = pct; hp.fill_color[2] = 0.0f;
        graphics::drawRect(enemy->position.x - (hpBarWidth - hpBarWidth*pct)/2.0f, enemy->position.y - enemySize * 0.7f, hpBarWidth*pct, 3.0f, hp);
    }
    float projSize = m_tileSize * 0.3f;
    graphics::Brush proj;
    proj.fill_color[0] = 1.0f; proj.fill_color[1] = 0.5f; proj.fill_color[2] = 0.0f;
    for (const auto* p : m_projectiles) {
        if (p->isActive) graphics::drawDisk(p->position.x, p->position.y, projSize, proj);
    }
    for (Widget* widget : m_Allwidget) widget->draw();
    graphics::Brush txt; txt.fill_color[0] = 1.0f; txt.fill_color[1] = 1.0f; txt.fill_color[2] = 1.0f;
    graphics::setFont(m_fonts_path);
    if (m_waveInProgress) {
        graphics::drawText(850.0f, 30.0f, 25.0f, "Enemies: " + std::to_string(m_enemies.size() + m_enemiesToSpawn), txt);
    } else if (m_currentWave == 0) {
        graphics::drawText(780.0f, 30.0f, 25.0f, "Click START to begin Wave 1", txt);
    } else {
        graphics::drawText(750.0f, 30.0f, 25.0f, "Wave complete! Click START for next wave", txt);
    }
}
void Globalstate::update_gameover()
{
    m_Allwidget.push_back(new Button(Button::BACK, 780.0f, 600.0f, "", "MAIN MENU", 0, 360.0f, 80.0f, Widget::CENTER, Widget::CENTER, 35.0f));
}
void Globalstate::draw_gameover()
{
    graphics::Brush dim;
    dim.fill_color[0] = 0.0f; dim.fill_color[1] = 0.0f; dim.fill_color[2] = 0.0f;
    dim.fill_opacity = 0.7f;
    graphics::drawRect(960.0f, 540.0f, 1920.0f, 1080.0f, dim);
    graphics::Brush txt;
    txt.fill_color[0] = 1.0f; txt.fill_color[1] = 0.2f; txt.fill_color[2] = 0.2f;
    graphics::setFont(m_fonts_path);
    graphics::drawText(750.0f, 400.0f, 80.0f, "GAME OVER", txt);
    txt.fill_color[0] = 1.0f; txt.fill_color[1] = 1.0f; txt.fill_color[2] = 1.0f;
    graphics::drawText(800.0f, 500.0f, 40.0f, "Wave Reached: " + std::to_string(m_currentWave), txt);
    for (Widget* widget : m_Allwidget) widget->draw();
}
void Globalstate::init()
{
    m_canvas_width = 1920.0f;
    m_canvas_height = 1080.0f;
    m_current_mode = GameMode::MENU;
}
void Globalstate::draw()
{
    switch (m_current_mode) {
    case GameMode::MENU: draw_menu(); break;
    case GameMode::DIFFICULTY_SELECT: draw_difficulty_select(); break;
    case GameMode::PLAYINGMODE: draw_playingmode(); break;
    case GameMode::GAME_OVER: draw_gameover(); break;
    }
}
void Globalstate::update(float dt)
{
    graphics::getMouseState(m_mouse);
    static GameMode last_mode = m_current_mode;
    for (Widget* widget : m_Allwidget) {
        widget->update(getMouse_pos_x(), getMouse_pos_y(), m_mouse.button_left_pressed);
    }
    bool mode_changed = false;
    for (Widget* widget : m_Allwidget) {
        Button* button = dynamic_cast<Button*>(widget);
        if (button && button->check_if_clicked()) {
            switch (button->getButtonType()) {
            case Button::PLAY:
                m_current_mode = GameMode::DIFFICULTY_SELECT;
                mode_changed = true;
                break;
            case Button::QUIT:
                graphics::stopMessageLoop();
                break;
            case Button::DIFFICULTY_EASY:
                setDifficulty(EASY); startGame();
                m_current_mode = GameMode::PLAYINGMODE;
                mode_changed = true;
                break;
            case Button::DIFFICULTY_MEDIUM:
                setDifficulty(MEDIUM); startGame();
                m_current_mode = GameMode::PLAYINGMODE;
                mode_changed = true;
                break;
            case Button::DIFFICULTY_HARD:
                setDifficulty(HARD); startGame();
                m_current_mode = GameMode::PLAYINGMODE;
                mode_changed = true;
                break;
            case Button::BACK:
                m_current_mode = GameMode::MENU;
                mode_changed = true;
                break;
            case Button::START_WAVE:
                if (!m_waveInProgress) nextWave();
                break;
            case Button::Tower_button:
                if (button->getGoldWorth() == 100) m_selectedTowerType = 0;
                else if (button->getGoldWorth() == 300) m_selectedTowerType = 1;
                else if (button->getGoldWorth() == 800) m_selectedTowerType = 2;
                else if (button->getGoldWorth() == 2000) m_selectedTowerType = 3;
                break;
            default: break;
            }
            if (mode_changed) break;
        }
    }
    if (m_current_mode == GameMode::PLAYINGMODE && m_selectedTowerType >= 0) {
        if (m_mouse.button_left_pressed && getMouse_pos_y() < 880.0f) {
            int gridX, gridY;
            screenToGrid(getMouse_pos_x(), getMouse_pos_y(), gridX, gridY);
            if (canPlaceTower(gridX, gridY) && m_gold >= getTowerCost(m_selectedTowerType)) {
                placeTower(gridX, gridY, m_selectedTowerType);
                m_selectedTowerType = -1;
            }
        }
        if (m_mouse.button_right_pressed) m_selectedTowerType = -1;
    }
    if (mode_changed || last_mode != m_current_mode) {
        clearWidgets();
        last_mode = m_current_mode;
    }
    if (m_Allwidget.empty()) {
        switch (m_current_mode) {
        case GameMode::MENU: update_menu(); break;
        case GameMode::DIFFICULTY_SELECT: update_difficulty_select(); break;
        case GameMode::PLAYINGMODE: update_playingmode(); break;
        case GameMode::GAME_OVER: update_gameover(); break;
        }
    }
    if (m_current_mode == GameMode::PLAYINGMODE) {
        if (m_waveInProgress && m_enemiesToSpawn > 0) {
            m_enemySpawnTimer += dt;
            if (m_enemySpawnTimer >= 0.8f) {
                spawnEnemy();
                m_enemiesToSpawn--;
                m_enemySpawnTimer = 0.0f;
            }
        }
        if (m_waveInProgress && m_enemiesToSpawn == 0 && m_enemies.empty()) {
            m_waveInProgress = false;
            m_gold += 50 + m_currentWave * 10;
        }
        updateEnemies(dt);
        updateTowers(dt);
        updateProjectiles(dt);
        for (Widget* widget : m_Allwidget) {
            Button* btn = dynamic_cast<Button*>(widget);
            if (btn) {
                if (btn->getButtonType() == Button::HEALTH) btn->set_font_text(std::to_string(m_lives));
                else if (btn->getButtonType() == Button::INVENTORY) btn->set_font_text(std::to_string(m_gold));
                else if (btn->getButtonType() == Button::PAUSE) btn->set_font_text("Wave " + std::to_string(m_currentWave));
            }
        }
    }
}
void Globalstate::Run()
{
    init();
    graphics::createWindow(static_cast<int>(m_canvas_width), static_cast<int>(m_canvas_height), "Aegis Tower Defense");
    graphics::setCanvasSize(m_canvas_width, m_canvas_height);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);
    graphics::setDrawFunction([]() { Globalstate::getInstance()->draw(); });
    graphics::setUpdateFunction([](float dt) { Globalstate::getInstance()->update(dt); });
    graphics::startMessageLoop();
    graphics::destroyWindow();
}
