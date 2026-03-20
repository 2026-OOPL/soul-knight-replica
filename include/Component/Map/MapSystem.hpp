#ifndef MAP_BASE_HPP
#define MAP_BASE_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "Component/Mob/Mob.hpp"
#include "Component/Player/Player.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Component/Camera/Camera.hpp"
#include "Scene.hpp"


class MapSystem : public Scene {
public:
    MapSystem();

    MapSystem(std::vector<std::shared_ptr<MapPiece>> pieces);

    virtual ~MapSystem() = default;

protected:
    std::vector<std::shared_ptr<Mob>> m_Mobs;
    std::vector<std::shared_ptr<Player>> m_Players;
    std::vector<std::shared_ptr<Camera>> m_Cameras;
    std::vector<std::shared_ptr<MapPiece>> m_Pieces;

    std::shared_ptr<Camera> m_AttachCamera;
};

#endif