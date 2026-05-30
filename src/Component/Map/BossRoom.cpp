#include <algorithm>
#include <array>
#include <memory>

#include "Component/Map/BossRoom.hpp"

#include "Common/Enums.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Prop/MechanicalRuinsObstacle.hpp"

namespace {

constexpr float kBossCornerObstacleInset = 64.0F;

} // namespace

BossRoom::BossRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
) : FightRoom(
        absolutePosition,
        doorConfig,
        info,
        wallThickness,
        RoomPurpose::BOSS
    ) {
}

void BossRoom::Initialize(MapSystem *mapSystem) {
    FightRoom::Initialize(mapSystem);
    this->SpawnCornerObstacles(mapSystem);
}

void BossRoom::SpawnCornerObstacles(MapSystem *mapSystem) {
    if (mapSystem == nullptr) {
        return;
    }

    const glm::vec2 roomCenter = this->GetAbsoluteTranslation();
    const glm::vec2 halfRoomSize = this->GetAreaSize() / 2.0F;
    const glm::vec2 cornerOffset = {
        std::max(0.0F, halfRoomSize.x - kBossCornerObstacleInset),
        std::max(0.0F, halfRoomSize.y - kBossCornerObstacleInset)
    };

    const std::array<glm::vec2, 4> positions = {
        roomCenter + glm::vec2{-cornerOffset.x, cornerOffset.y},
        roomCenter + glm::vec2{cornerOffset.x, cornerOffset.y},
        roomCenter + glm::vec2{-cornerOffset.x, -cornerOffset.y},
        roomCenter + glm::vec2{cornerOffset.x, -cornerOffset.y}
    };

    MechanicalRuinsObstacleConfig config;
    config.owningRoom = this->shared_from_this();

    for (const glm::vec2 &position : positions) {
        mapSystem->AddProp(std::make_shared<MechanicalRuinsPillar>(
            position,
            config
        ));
    }
}
