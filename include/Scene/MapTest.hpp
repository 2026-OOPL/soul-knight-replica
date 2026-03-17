#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <vector>

#include "Component/Map/MapSystem.hpp"

class MapTest : public MapSystem {
public:
    MapTest(glm::vec2 cooridinate) : MapSystem(cooridinate, {}) {
        m_Transform.scale = {1.5, 1.5};
        this->m_Cooridinate = cooridinate;
    }

    ~MapTest() override { Dispose(); }

    void Initialize() override;
    void Dispose() override;
    void Update() override;

private:
    bool WillPlayerCollide(const glm::vec2 &nextCoordinate) const;

private:
    glm::vec2 m_Cooridinate;

    float m_PlayerSpeed = 0.35F;
};

#endif
