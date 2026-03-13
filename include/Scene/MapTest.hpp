#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include "Scene.hpp"
#include "Util/GameObject.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <vector>
#include "Map/MapPiece.hpp"

class MapTest : public Scene {
public:
    MapTest(Util::Renderer* m_Root) : Scene(m_Root) {
        Initialize();
    }

    ~MapTest() override {
        Dispose();
    };

    void Initialize() override;
    void Dispose() override;

    void Update() override;

    std::vector<MapPiece> pieces;
    glm::vec2 cooridinate;

protected:
    // std::unique_ptr<Util::GameObject> m_Character = 
};

// Coding
#endif