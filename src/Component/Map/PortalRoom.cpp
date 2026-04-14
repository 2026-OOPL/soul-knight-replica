#include "Component/Map/PortalRoom.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Prop/Portal.hpp"
#include <memory>

PortalRoom::PortalRoom(
    const glm::vec2 &absolutePosition,
    const DoorConfig &doorConfig,
    const std::shared_ptr<RoomInfo> &info,
    float wallThickness
)
: BaseRoom(
    absolutePosition,
    info->GetRoomType(),
    RoomPurpose::PORTAL,
    doorConfig,
    BaseRoom::BuildWallConfigFromDoorConfig(
        doorConfig,
        wallThickness,
        info->GetRoomType(),
        RoomPurpose::PORTAL
    )
) {

}

void PortalRoom::OnPlayerEnter() {
    return;
}

void PortalRoom::OnPlayerLeave() {
    return;
}

void PortalRoom::Initialize(MapSystem* mapSystem) {
    std::shared_ptr<Portal> portal = std::make_shared<Portal>(this->GetAbsoluteTranslation());
    mapSystem->AddProp(portal);
}

void PortalRoom::Update() {
    BaseRoom::Update();
}
