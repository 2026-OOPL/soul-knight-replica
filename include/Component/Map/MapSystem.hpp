#ifndef MAP_BASE_HPP
#define MAP_BASE_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "Component/Camera/Camera.hpp"
#include "Component/IMapObject.hpp"
#include "Component/Mob/Mob.hpp"
#include "Component/Player/Player.hpp"
#include "Util/GameObject.hpp"
#include "Scene.hpp"
#include "Component/Map/MapPiece.hpp"

class MapSystem : public Scene {
public:
    MapSystem(glm::vec2 cooridinate, std::vector<std::shared_ptr<MapPiece>> pieces) : Scene() {
        this->m_Pieces = pieces;
    }

    virtual ~MapSystem() = default;

    // 從相機取得目前畫面位置，並
    void GetCameraCoor();

protected:
    std::vector<std::shared_ptr<Mob>> m_Mob;
    std::vector<std::shared_ptr<Player>> m_Player;
    std::vector<std::shared_ptr<Camera>> m_Camera;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;

    std::shared_ptr<Camera> m_CameraFocus;
};

#endif