#pragma once

#include "../ecs/Registry.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

namespace Nightfall {

/**
 * @brief 波次系统 - 管理敌人生成波次
 * 
 * 功能：
 * - 定时生成僵尸波次
 * - 每波难度递增
 * - 在地图边缘随机位置生成敌人
 */
class WaveSystem {
public:
    WaveSystem();
    ~WaveSystem();

    void init(const sf::FloatRect& spawnArea);
    void update(float deltaTime, Registry& registry);
    
    // 手动触发下一波
    void startNextWave(Registry& registry);
    
    // 查询
    int getCurrentWave() const { return m_currentWave; }
    int getEnemiesRemaining() const { return m_enemiesRemaining; }
    bool isWaveActive() const { return m_waveActive; }
    float getTimeUntilNextWave() const { return m_timeBetweenWaves - m_waveTimer; }

private:
    void spawnWave(Registry& registry);
    void spawnZombie(Registry& registry, const sf::Vector2f& position, ZombieType type);
    sf::Vector2f getRandomSpawnPosition();
    
private:
    sf::FloatRect m_spawnArea;      // 生成区域
    int m_currentWave{0};            // 当前波次
    int m_enemiesRemaining{0};       // 剩余敌人数量
    bool m_waveActive{false};        // 当前波次是否激活
    float m_waveTimer{0.f};          // 波次计时器
    float m_timeBetweenWaves{30.f}; // 波次间隔时间（秒）
    float m_spawnDelay{1.f};         // 每个敌人生成间隔
    float m_spawnTimer{0.f};         // 生成计时器
    
    // 当前波次的生成队列
    struct SpawnEntry {
        ZombieType type;
        sf::Vector2f position;
    };
    std::vector<SpawnEntry> m_spawnQueue;
};

} // namespace Nightfall
