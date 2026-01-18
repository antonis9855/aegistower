#include "globalstate.h"
#include "sgg/graphics.h"
#include "display.h"
#include "button.h"
#include "astar.h"
#include "collisions.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
// Main game loop kai logic






Globalstate* Globalstate::m_instance = nullptr;


Globalstate::Globalstate()
    : m_canvas_width(1920.0f), m_canvas_height(1080.0f),
      m_current_mode(GameMode::MENU), m_difficulty(MEDIUM),
      m_gold(0), m_lives(0), m_currentWave(0), m_waveInProgress(false),
      m_selectedTowerType(-1), m_enemySpawnTimer(0.0f),
      m_enemiesToSpawn(0), m_nextEnemyId(0), m_nextTowerId(0), m_nextProjectileId(0),
      m_gridWidth(0), m_gridHeight(0), m_tileSizeX(0.0f), m_tileSizeY(0.0f), m_mapOffsetX(0), m_mapOffsetY(0),
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
    p.x = m_mapOffsetX + gridX * m_tileSizeX + m_tileSizeX / 2.0f;
    p.y = m_mapOffsetY + gridY * m_tileSizeY + m_tileSizeY / 2.0f;
    return p;
}
void Globalstate::screenToGrid(float screenX, float screenY, int& gridX, int& gridY) const
{
    gridX = static_cast<int>((screenX - m_mapOffsetX) / m_tileSizeX);
    gridY = static_cast<int>((screenY - m_mapOffsetY) / m_tileSizeY);
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
Point Globalstate::getNodePosition(int nodeId) const
{
    for (const auto& node : m_pathNodes) {
        if (node.id == nodeId) return node.position;
    }
    return {};
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
    // Fill map me default GRASS
    const float uiHeight = 100.0f;
    float playAreaHeight = m_canvas_height - uiHeight;
    float playAreaWidth = m_canvas_width;
    m_tileSizeX = playAreaWidth / m_gridWidth;
    m_tileSizeY = playAreaHeight / m_gridHeight;
    m_mapOffsetX = 0.0f;
    m_mapOffsetY = 0.0f;
    // Build maze-like path me 1-tile width
    auto setPath = [this](int x, int y) {
        if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight) {
            m_mapData[y][x] = PATH;
        }
    };
    auto hLine = [&setPath](int y, int x0, int x1) {
        if (x0 > x1) std::swap(x0, x1);
        for (int x = x0; x <= x1; x++) setPath(x, y);
    };
    auto vLine = [&setPath](int x, int y0, int y1) {
        if (y0 > y1) std::swap(y0, y1);
        for (int y = y0; y <= y1; y++) setPath(x, y);
    };

    int spawnY = m_gridHeight / 2;
    int midY = m_gridHeight / 2;
    int topY = 8;
    int bottomY = m_gridHeight - 9;
    int yA = 16;
    int yB = 26;
    int yC = 36;
    int yD = 46;
    int x1 = 8;
    int x2 = 18;
    int x3 = 30;
    int x4 = 42;
    int x5 = 54;

    hLine(midY, 0, m_gridWidth - 1);
    hLine(topY, x1, x5);
    hLine(bottomY, x1, x5);
    vLine(x1, topY, bottomY);
    vLine(x3, topY, bottomY);
    vLine(x5, topY, bottomY);

    hLine(yA, x1, x3);
    hLine(yB, x3, x5);
    hLine(yC, x1, x3);
    hLine(yD, x3, x5);

    vLine(x2, yA, yC);
    vLine(x4, yB, yD);
    vLine(x2, topY, yA);
    vLine(x4, yD, bottomY);

    hLine(yA + 4, x2, x4);
    hLine(yC - 4, x2, x4);

    m_mapData[spawnY][0] = SPAWN;
    m_mapData[spawnY][m_gridWidth - 1] = CASTLE;

    for (int y = 6; y < 14; y++) {
        for (int x = 6; x < 14; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 14; y < 22; y++) {
        for (int x = 40; x < 50; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 34; y < 44; y++) {
        for (int x = 12; x < 22; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 38; y < 50; y++) {
        for (int x = 44; x < 54; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    for (int y = 22; y < 32; y++) {
        for (int x = 26; x < 38; x++) {
            if (m_mapData[y][x] == GRASS) m_mapData[y][x] = ROCK;
        }
    }
    m_towerPlaced.clear();
    m_towerPlaced.resize(m_gridHeight, std::vector<bool>(m_gridWidth, false));
    buildPathGraph();
    nextWave();
} 
void Globalstate::nextWave()
{
    m_currentWave++;
    m_waveInProgress = true;
    // +2 enemies ana wave, arxi me 7
    m_enemiesToSpawn = 7 + (m_currentWave - 1) * 2;
    m_enemySpawnTimer = 0.0f;
    Astar::updateNodeWeights(m_pathNodes, m_towers);

    std::cout << "[DEBUG] Starting wave " << m_currentWave << " with " << m_enemiesToSpawn << " enemies to spawn" << std::endl;

    spawnEnemy();
    m_enemiesToSpawn--;

    std::cout << "[DEBUG] First enemy spawned, " << m_enemiesToSpawn << " remaining" << std::endl;
}
void Globalstate::spawnEnemy()
{
    Enemy* enemy = new Enemy();
    enemy->id = m_nextEnemyId++;
    enemy->isAlive = true;
    enemy->pathIndex = 0;
    enemy->maxHealth = (50.0f + m_currentWave * 20.0f) * m_diffSettings.enemyHealthMult;
    enemy->health = enemy->maxHealth;
    enemy->speed = (70.0f + m_currentWave * 3.0f) * m_diffSettings.enemySpeedMult;
    enemy->randomWeight = static_cast<float>(std::rand() % 50) / 10.0f;
    // Vres path apo spawn se castle
    enemy->path = Astar::findPath(m_spawnNodeId, m_castleNodeId, m_pathNodes, enemy->randomWeight);
    if (!enemy->path.empty()) {
        enemy->position = getNodePosition(enemy->path[0]);
    }
    m_enemies.push_back(enemy);
}
void Globalstate::updateEnemies(float dt)
{
    for (auto* enemy : m_enemies) {
        if (!enemy->isAlive) continue;
        if (enemy->pathIndex < static_cast<int>(enemy->path.size())) {
            int targetNodeId = enemy->path[enemy->pathIndex];
            Point targetPos = getNodePosition(targetNodeId);
            float dx = targetPos.x - enemy->position.x;
            float dy = targetPos.y - enemy->position.y;
            float dist = Collisions::distance(enemy->position, targetPos);
            float moveSpeed = enemy->speed * (dt / 1000.0f);

            if (dist <= moveSpeed) {
                enemy->position = targetPos;
                enemy->pathIndex++;
            } else {
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
    float tileScale = (m_tileSizeX < m_tileSizeY) ? m_tileSizeX : m_tileSizeY;
    Tower* tower = new Tower();
    tower->id = m_nextTowerId++;
    tower->towerType = towerType;
    Point tpos = gridToScreen(gridX, gridY);
    tower->position.x = tpos.x;
    tower->position.y = tpos.y;
    tower->timeSinceLastShot = 0.0f;
    tower->cost = cost;
    switch (towerType) {
    case 0: tower->range = tileScale * 4;  tower->fireRate = 500.0f; tower->damage = 15.0f; break;
    case 1: tower->range = tileScale * 5;  tower->fireRate = 800.0f; tower->damage = 35.0f; break;
    case 2: tower->range = tileScale * 6;  tower->fireRate = 1200.0f; tower->damage = 80.0f; break;
    case 3: tower->range = tileScale * 8;  tower->fireRate = 600.0f; tower->damage = 120.0f; break;
    }
    m_towerPlaced[gridY][gridX] = true;
    m_towers.push_back(tower);


    std::cout << "[DEBUG] Tower placed at grid(" << gridX << "," << gridY
              << ") -> screen(" << tower->position.x << "," << tower->position.y << ")" << std::endl;
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
                proj->position.x = tower->position.x;
                proj->position.y = tower->position.y;
                proj->targetEnemyId = target->id;
                proj->speed = 600.0f;
                proj->damage = tower->damage;
                proj->isActive = true;
                m_projectiles.push_back(proj);
                tower->timeSinceLastShot = 0.0f;


                std::cout << "[DEBUG] Projectile spawned at (" << proj->position.x << "," << proj->position.y
                          << ") from tower at (" << tower->position.x << "," << tower->position.y << ")" << std::endl;
            }
        }
    }
}
void Globalstate::updateProjectiles(float dt)
{
    for (auto* proj : m_projectiles) {
        if (!proj->isActive) continue;
        // Kinisi pros target kai hit check
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
        float moveSpeed = proj->speed * (dt / 1000.0f);
        if (dist <= 15.0f || dist <= moveSpeed) {
            target->health -= proj->damage;
            m_gold += static_cast<int>(10 * m_diffSettings.goldMult);
            proj->isActive = false;
            if (target->health <= 0) {
                target->isAlive = false;
                m_gold += static_cast<int>(10 * m_diffSettings.goldMult);
            }
        } else {
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
            graphics::drawRect(pos.x, pos.y, m_tileSizeX, m_tileSizeY, brush);
        }
    }
}
void Globalstate::update_menu()
{
    m_Allwidget.push_back(new ResourceDisplay("menu_backround.png", false, 0.0f, 0.0f, 1920.0f, 1080.0f));
    m_Allwidget.push_back(new Button(Button::PLAY, 780.0f, 400.0f, "play_button.png", "", 0, 360.0f, 100.0f, Widget::CENTER, Widget::CENTER, 40.0f));
    m_Allwidget.push_back(new Button(Button::QUIT, 780.0f, 520.0f, "quit_button.png", "QUIT", 0, 360.0f, 100.0f, Widget::CENTER, Widget::CENTER, 40.0f));
}
void Globalstate::draw_menu()
{
    for (Widget* widget : m_Allwidget) widget->draw();
}
void Globalstate::update_playingmode()
{
    const float uiHeight = 100.0f;
    const float uiY = m_canvas_height - uiHeight;
    m_Allwidget.push_back(new ResourceDisplay("", false, 0.0f, uiY, 1920.0f, uiHeight));
    float start_x = 100.0f;
    float spacing = 130.0f;
    float y = uiY + (uiHeight - 90.0f) / 2.0f;
    float ySmall = uiY + (uiHeight - 70.0f) / 2.0f;
    float yTiny = uiY + (uiHeight - 40.0f) / 2.0f;
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x, y, "BlueTower.png", "100", 100, 90.0f, 90.0f, Widget::CENTER, Widget::UP, 16.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + spacing, y, "Purple_tower.png", "300", 300, 90.0f, 90.0f, Widget::CENTER, Widget::UP, 16.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 2*spacing, y, "RedTower.png", "800", 800, 90.0f, 90.0f, Widget::CENTER, Widget::UP, 16.0f));
    m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 3*spacing, y, "GoldenTower.png", "2000", 2000, 90.0f, 90.0f, Widget::CENTER, Widget::UP, 16.0f));
    m_Allwidget.push_back(new Button(Button::HEALTH, 1350.0f, ySmall, "red-pixel-heart.png", std::to_string(m_lives), 0, 70.0f, 70.0f, Widget::RIGHT, Widget::CENTER, 30.0f, 194.0f, 151.0f, 112.0f, false));
    m_Allwidget.push_back(new Button(Button::INVENTORY, 1550.0f, ySmall, "coins.png", std::to_string(m_gold), 0, 70.0f, 70.0f, Widget::RIGHT, Widget::CENTER, 30.0f, 194.0f, 151.0f, 112.0f, false));
    m_Allwidget.push_back(new Button(Button::PAUSE, 1750.0f, yTiny, "", "Wave " + std::to_string(m_currentWave), 0, 110.0f, 40.0f, Widget::CENTER, Widget::CENTER, 22.0f, 194.0f, 151.0f, 112.0f, false));
}
void Globalstate::draw_playingmode()
{
    const float uiHeight = 100.0f;
    const float uiY = m_canvas_height - uiHeight;
    float tileScale = (m_tileSizeX < m_tileSizeY) ? m_tileSizeX : m_tileSizeY;
    drawMap();

    float towerSize = tileScale * 0.9f;
    graphics::Brush towerBrush;
    towerBrush.fill_opacity = 1.0f;
    std::string textures[] = { "BlueTower.png", "Purple_tower.png", "RedTower.png", "GoldenTower.png" };
    for (const auto* tower : m_towers) {
        towerBrush.texture = m_images_path + textures[tower->towerType];
        graphics::drawRect(tower->position.x, tower->position.y, towerSize, towerSize, towerBrush);
    }


    float enemySize = tileScale * 0.7f;
    graphics::Brush enemyBrush;
    enemyBrush.fill_opacity = 1.0f;
    enemyBrush.outline_opacity = 0.0f;
    enemyBrush.texture = m_images_path + "enemies/goblin.png";
    for (const auto* enemy : m_enemies) {
        if (!enemy->isAlive) continue;
        graphics::drawRect(enemy->position.x, enemy->position.y, enemySize, enemySize, enemyBrush);
        float hpBarWidth = tileScale * 0.8f;
        graphics::Brush hpBg; hpBg.fill_color[0] = 0.3f; hpBg.fill_color[1] = 0.3f; hpBg.fill_color[2] = 0.3f;
        graphics::drawRect(enemy->position.x, enemy->position.y - enemySize * 0.7f, hpBarWidth + 2, 4.0f, hpBg);
        graphics::Brush hp;
        float pct = enemy->health / enemy->maxHealth;
        hp.fill_color[0] = 1.0f - pct; hp.fill_color[1] = pct; hp.fill_color[2] = 0.0f;
        graphics::drawRect(enemy->position.x - (hpBarWidth - hpBarWidth * pct) / 2.0f, enemy->position.y - enemySize * 0.7f, hpBarWidth * pct, 3.0f, hp);
    }

    float projSize = tileScale * 0.3f;
    graphics::Brush proj;
    proj.fill_color[0] = 1.0f; proj.fill_color[1] = 0.5f; proj.fill_color[2] = 0.0f;
    for (const auto* p : m_projectiles) {
        if (p->isActive) graphics::drawDisk(p->position.x, p->position.y, projSize, proj);
    }
    for (Widget* widget : m_Allwidget) widget->draw();
    graphics::Brush txt; txt.fill_color[0] = 1.0f; txt.fill_color[1] = 1.0f; txt.fill_color[2] = 1.0f;
    graphics::setFont(m_fonts_path);
    graphics::drawText(820.0f, 30.0f, 25.0f, "Wave " + std::to_string(m_currentWave), txt);
    graphics::drawText(760.0f, 60.0f, 22.0f, "Enemies: " + std::to_string(m_enemies.size() + m_enemiesToSpawn), txt);

    if (m_selectedTowerType >= 0 && getMouse_pos_y() < uiY) {
        graphics::Brush rangeBrush;
        rangeBrush.fill_opacity = 0.3f;
        rangeBrush.fill_color[0] = 0.0f;
        rangeBrush.fill_color[1] = 1.0f;
        rangeBrush.fill_color[2] = 0.0f;
        rangeBrush.outline_opacity = 1.0f;
        rangeBrush.outline_color[0] = 0.0f;
        rangeBrush.outline_color[1] = 1.0f;
        rangeBrush.outline_color[2] = 0.0f;
        rangeBrush.outline_width = 3.0f;
        float ranges[] = { tileScale * 4, tileScale * 5, tileScale * 6, tileScale * 8 };
        graphics::drawDisk(getMouse_pos_x(), getMouse_pos_y(), ranges[m_selectedTowerType], rangeBrush);
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
    case GameMode::PLAYINGMODE: draw_playingmode(); break;
    case GameMode::GAME_OVER: draw_gameover(); break;
    }
}
void Globalstate::update(float dt)
{
    graphics::getMouseState(m_mouse);
    static GameMode last_mode = m_current_mode;
    static bool lastMouseState = false;
    static int towerJustSelected = -1;
    for (Widget* widget : m_Allwidget) {
        widget->update(getMouse_pos_x(), getMouse_pos_y(), m_mouse.button_left_pressed);
    }
    bool mode_changed = false;
    for (Widget* widget : m_Allwidget) {
        Button* button = dynamic_cast<Button*>(widget);
        if (button && button->check_if_clicked()) {
            switch (button->getButtonType()) {
            case Button::PLAY:
                startGame();
                m_current_mode = GameMode::PLAYINGMODE;
                mode_changed = true;
                break;
            case Button::QUIT:
                graphics::stopMessageLoop();
                break;
            case Button::BACK:
                m_current_mode = GameMode::MENU;
                mode_changed = true;
                break;
            case Button::Tower_button:
                if (button->getGoldWorth() == 100) { m_selectedTowerType = 0; towerJustSelected = 0; }
                else if (button->getGoldWorth() == 300) { m_selectedTowerType = 1; towerJustSelected = 1; }
                else if (button->getGoldWorth() == 800) { m_selectedTowerType = 2; towerJustSelected = 2; }
                else if (button->getGoldWorth() == 2000) { m_selectedTowerType = 3; towerJustSelected = 3; }
                break;
            default: break;
            }
            if (mode_changed) break;
        }
    }

    if (m_current_mode == GameMode::PLAYINGMODE) {
        const float uiHeight = 100.0f;
        const float uiY = m_canvas_height - uiHeight;
        bool mouseY_inGame = getMouse_pos_y() < uiY;
        bool mouseClicked = m_mouse.button_left_pressed && !lastMouseState;

        if (towerJustSelected >= 0) {
            towerJustSelected = -1;
        } else if (m_selectedTowerType >= 0) {
            if (mouseClicked && mouseY_inGame) {
                int gridX, gridY;
                screenToGrid(getMouse_pos_x(), getMouse_pos_y(), gridX, gridY);
                if (canPlaceTower(gridX, gridY) && m_gold >= getTowerCost(m_selectedTowerType)) {
                    placeTower(gridX, gridY, m_selectedTowerType);
                    m_selectedTowerType = -1;
                }
            }

            if (m_mouse.button_right_pressed) {
                m_selectedTowerType = -1;
            }
        }

        lastMouseState = m_mouse.button_left_pressed;
    }
    if (mode_changed || last_mode != m_current_mode) {
        clearWidgets();
        last_mode = m_current_mode;
    }
    if (m_Allwidget.empty()) {
        switch (m_current_mode) {
        case GameMode::MENU: update_menu(); break;
        case GameMode::PLAYINGMODE: update_playingmode(); break;
        case GameMode::GAME_OVER: update_gameover(); break;
        }
    }
    if (m_current_mode == GameMode::PLAYINGMODE) {
        if (m_waveInProgress && m_enemiesToSpawn > 0) {
            m_enemySpawnTimer += dt;


            static int debugFrameCount = 0;
            if (debugFrameCount < 10) {
                std::cout << "[DEBUG] Frame " << debugFrameCount << ": dt=" << dt
                          << "ms, timer=" << m_enemySpawnTimer << "ms, toSpawn=" << m_enemiesToSpawn << std::endl;
                debugFrameCount++;
            }

            bool spawnClear = true;
            if (m_spawnNodeId >= 0) {
                Point spawnPos = getNodePosition(m_spawnNodeId);
                float tileScale = (m_tileSizeX < m_tileSizeY) ? m_tileSizeX : m_tileSizeY;
                float minSpawnSpacing = tileScale * 0.8f;
                for (const auto* enemy : m_enemies) {
                    if (!enemy->isAlive) continue;
                    if (Collisions::distance(spawnPos, enemy->position) < minSpawnSpacing) {
                        spawnClear = false;
                        break;
                    }
                }
            }
            if (m_enemySpawnTimer >= 1000.0f && spawnClear) {
                std::cout << "[DEBUG] Spawning enemy! timer=" << m_enemySpawnTimer << std::endl;
                spawnEnemy();
                m_enemiesToSpawn--;
                m_enemySpawnTimer = 0.0f;
            }
        }
        if (m_waveInProgress && m_enemiesToSpawn == 0 && m_enemies.empty()) {
            m_waveInProgress = false;
            m_gold += 50 + m_currentWave * 10;
            if (m_lives > 0) nextWave();
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
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_STRETCH);
    graphics::setDrawFunction([]() { Globalstate::getInstance()->draw(); });
    graphics::setUpdateFunction([](float dt) { Globalstate::getInstance()->update(dt); });
    graphics::startMessageLoop();
    graphics::destroyWindow();


}
