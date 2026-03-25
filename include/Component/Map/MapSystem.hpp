#ifndef MAP_SYSTEM_HPP
#define MAP_SYSTEM_HPP

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Component/Camera/Camera.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Component/Mob/Mob.hpp"
#include "Component/Player/Player.hpp"
#include "Scene.hpp"

class MapSystem : public Scene {
public:
    MapSystem();

    explicit MapSystem(std::vector<std::shared_ptr<MapPiece>> pieces);

    ~MapSystem() override = default;

    bool IsPlayerInsideRoom() const;
    glm::vec2 GetCameraCoor() const;

protected:
    std::vector<std::shared_ptr<Mob>> m_Mobs;
    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<Camera>> m_Cameras;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;
    std::shared_ptr<Camera> m_AttachCamera;
    float m_RoomWallThickness = 20.0F;
};

#endif
