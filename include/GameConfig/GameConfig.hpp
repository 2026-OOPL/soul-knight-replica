#ifndef GAME_CONFIG_HPP
#define GAME_CONFIG_HPP

class GameConfig {
public:
    static GameConfig& GetInstance() {
        static GameConfig instance;
        return instance;
    }

    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    int max_connections = 100;

    float m_BGMVolume = 0.5F;
    float m_SFXVolume = 0.5F;


private:
    GameConfig() = default;
};

#endif