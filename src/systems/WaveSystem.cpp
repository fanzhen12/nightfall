#include "WaveSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include <cmath>
#include <cstdlib>

namespace Nightfall {

WaveSystem::WaveSystem() {
}

WaveSystem::~WaveSystem() {
    NF_INFO("Wave system shutdown");
}

void WaveSystem::init(const sf::FloatRect& spawnArea) {
    m_spawnArea = spawnArea;
    NF_INFO("Wave system initialized. Spawn area: ({}, {}) - {}x{}", 
            spawnArea.position.x, spawnArea.position.y, 
            spawnArea.size.x, spawnArea.size.y);
}

void WaveSystem::update(float deltaTime, Registry& registry) {
    if (!m_waveActive) {
        // 等待下一波
        m_waveTimer += deltaTime;
        
        if (m_waveTimer >= m_timeBetweenWaves) {
            startNextWave(registry);
        }
    } else {
        // 波次进行中
        // 从生成队列中生成敌人
        if (!m_spawnQueue.empty()) {
            m_spawnTimer += deltaTime;
            
            if (m_spawnTimer >= m_spawnDelay) {
                auto entry = m_spawnQueue.back();
                m_spawnQueue.pop_back();
                spawnZombie(registry, entry.position, entry.type);
                m_spawnTimer = 0.f;
            }
        }
        
        // 检查是否所有敌人都被消灭
        auto view = registry.view<Zombie, Hostile>();
        int aliveEnemies = 0;
        for (auto entity : view) {
            if (auto* health = registry.tryGetComponent<Health>(entity)) {
                if (!health->isDead()) {
                    aliveEnemies++;
                }
            }
        }
        
        m_enemiesRemaining = aliveEnemies;
        
        // 波次完成
        if (m_enemiesRemaining == 0 && m_spawnQueue.empty()) {
            m_waveActive = false;
            m_waveTimer = 0.f;
            NF_INFO("Wave {} completed! Next wave in {} seconds", m_currentWave, m_timeBetweenWaves);
        }
    }
}

void WaveSystem::startNextWave(Registry& registry) {
    m_currentWave++;
    m_waveActive = true;
    m_waveTimer = 0.f;
    m_spawnQueue.clear();
    
    NF_INFO("Starting Wave {}!", m_currentWave);
    
    // 根据波次计算敌人数量和类型
    int normalCount = 3 + (m_currentWave - 1) * 2;  // 3, 5, 7, 9...
    int fastCount = (m_currentWave > 2) ? (m_currentWave - 2) : 0;  // 从第3波开始
    int tankCount = (m_currentWave > 4) ? (m_currentWave - 4) : 0;  // 从第5波开始
    
    // 生成普通僵尸
    for (int i = 0; i < normalCount; ++i) {
        m_spawnQueue.push_back({ZombieType::Normal, getRandomSpawnPosition()});
    }
    
    // 生成快速僵尸
    for (int i = 0; i < fastCount; ++i) {
        m_spawnQueue.push_back({ZombieType::Fast, getRandomSpawnPosition()});
    }
    
    // 生成坦克僵尸
    for (int i = 0; i < tankCount; ++i) {
        m_spawnQueue.push_back({ZombieType::Tank, getRandomSpawnPosition()});
    }
    
    m_enemiesRemaining = normalCount + fastCount + tankCount;
    
    NF_INFO("Wave {}: {} Normal, {} Fast, {} Tank zombies", 
            m_currentWave, normalCount, fastCount, tankCount);
}

void WaveSystem::spawnZombie(Registry& registry, const sf::Vector2f& position, ZombieType type) {
    auto zombie = registry.createZombie(position, type);
    NF_INFO("Spawned zombie type {} at ({}, {})", 
            static_cast<int>(type), position.x, position.y);
}

sf::Vector2f WaveSystem::getRandomSpawnPosition() {
    // 在地图边缘随机选择一个位置
    int edge = rand() % 4; // 0=上, 1=右, 2=下, 3=左
    
    float x, y;
    const float margin = 50.f; // 离边界的距离
    
    switch (edge) {
        case 0: // 上边缘
            x = m_spawnArea.position.x + (rand() % static_cast<int>(m_spawnArea.size.x));
            y = m_spawnArea.position.y - margin;
            break;
        case 1: // 右边缘
            x = m_spawnArea.position.x + m_spawnArea.size.x + margin;
            y = m_spawnArea.position.y + (rand() % static_cast<int>(m_spawnArea.size.y));
            break;
        case 2: // 下边缘
            x = m_spawnArea.position.x + (rand() % static_cast<int>(m_spawnArea.size.x));
            y = m_spawnArea.position.y + m_spawnArea.size.y + margin;
            break;
        case 3: // 左边缘
        default:
            x = m_spawnArea.position.x - margin;
            y = m_spawnArea.position.y + (rand() % static_cast<int>(m_spawnArea.size.y));
            break;
    }
    
    return sf::Vector2f(x, y);
}

} // namespace Nightfall
