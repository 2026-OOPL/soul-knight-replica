#ifndef BULLET_CONFIG_HPP
#define BULLET_CONFIG_HPP

#include <string>
#include <vector>

#include <glm/vec2.hpp>

struct BulletConfig {
    std::vector<std::string> sprites;
    glm::vec2 visualScale = {1.0F, 1.0F};
    glm::vec2 colliderSize = {12.0F, 12.0F};
    int zIndex = 1;
    bool loopAnimation = true;
    int frameIntervalMs = 20;
};

#endif
