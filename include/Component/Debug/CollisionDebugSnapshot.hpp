#ifndef COMPONENT_DEBUG_COLLISION_DEBUG_SNAPSHOT_HPP
#define COMPONENT_DEBUG_COLLISION_DEBUG_SNAPSHOT_HPP

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Map/RoomTransitionSystem.hpp"
#include "Util/Color.hpp"

namespace Core {
class Drawable;
}

class BaseRoom;
class Bullet;
class Camera;
class Character;
class Gangway;
class Player;

struct CollisionDebugEntry {
    glm::vec2 worldCenter = {0.0F, 0.0F};
    glm::vec2 worldSize = {0.0F, 0.0F};
    float worldRotation = 0.0F;
    Util::Color color = Util::Color(255, 255, 255, 255);
    std::string label;
};

struct CollisionDebugVisualEntry {
    std::shared_ptr<Core::Drawable> drawable = nullptr;
    glm::vec2 screenTranslation = {0.0F, 0.0F};
    glm::vec2 screenScale = {1.0F, 1.0F};
    float screenRotation = 0.0F;
};

struct CollisionDebugCameraState {
    glm::vec2 coordinate = {0.0F, 0.0F};
    glm::vec2 scale = {1.0F, 1.0F};
    float rotation = 0.0F;
};

struct CollisionDebugSnapshot {
    std::vector<CollisionDebugVisualEntry> visualEntries;
    std::vector<CollisionDebugEntry> entries;
    CollisionDebugCameraState cameraState;
};

CollisionDebugSnapshot BuildCollisionDebugSnapshot(
    const std::shared_ptr<BaseRoom> &currentRoom,
    const RoomTransitionSystem::DoorPassageContext &doorPassage,
    const std::vector<std::shared_ptr<Gangway>> &gangways,
    const std::vector<std::shared_ptr<Player>> &players,
    const std::vector<std::shared_ptr<Character>> &mobs,
    const std::vector<std::shared_ptr<Bullet>> &bullets,
    Camera *attachCamera
);

#endif
