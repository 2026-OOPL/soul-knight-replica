#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <functional>

enum class Direction {
    LEFT,
    TOP,
    RIGHT,
    BOTTOM
};

class MapGenerator {
public:
    MapGenerator(std::string seed) {
        // Initialize seed generator
        std::hash<std::string> hasher;
        this->m_Seed = hasher(seed);
        m_Engine = std::mt19937_64(m_Seed);
        m_Dist = std::uniform_real_distribution<double>(0.0, 1.0);

        // Reset current coordinate to generate from 0, 0
        m_CurrentCoordinate = {0, 0};
    };

    std::vector<std::string> rooms = {};

    Direction GetDirection();
    void GenerateRoom();

private:
    glm::vec2 m_CurrentCoordinate;

    unsigned int long long m_Seed;
    std::mt19937_64 m_Engine;
    std::uniform_real_distribution<double> m_Dist;
    
};

#endif