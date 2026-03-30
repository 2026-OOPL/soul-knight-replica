#include "Component/Map/Gangway.hpp"

#include <algorithm>

#include "Component/Map/BaseRoom.hpp"

namespace {

constexpr char kHorizontalGangwaySprite[] = RESOURCE_DIR "/Map/Gangway/Gangway_15x5.png";
constexpr char kVerticalGangwaySprite[] = RESOURCE_DIR "/Map/Gangway/Gangway_5x15.png";

} // namespace

Gangway::Gangway(const glm::vec2 &absolutePosition, Config config)
    : RectMapArea(
          absolutePosition + config.positionOffset,
          Gangway::ResolveGangwaySprite(config.orientation),
          Gangway::BuildAreaSize(config),
          Gangway::BuildWallConfig(config)
      ),
      m_Orientation(config.orientation) {
    this->m_AbsoluteTransform.scale = {1.0F, 1.0F};
    this->SetZIndex(-1.0F);
}

void Gangway::ConnectRooms(
    const std::shared_ptr<BaseRoom> &firstRoom,
    const std::shared_ptr<BaseRoom> &secondRoom
) {
    this->m_FirstRoom = firstRoom;
    this->m_SecondRoom = secondRoom;
}

bool Gangway::ConnectsRoom(const std::shared_ptr<BaseRoom> &room) const {
    if (room == nullptr) {
        return false;
    }

    const std::shared_ptr<BaseRoom> firstRoom = this->m_FirstRoom.lock();
    const std::shared_ptr<BaseRoom> secondRoom = this->m_SecondRoom.lock();

    return room == firstRoom || room == secondRoom;
}

std::shared_ptr<BaseRoom> Gangway::GetOtherRoom(const std::shared_ptr<BaseRoom> &room) const {
    if (room == nullptr) {
        return nullptr;
    }

    const std::shared_ptr<BaseRoom> firstRoom = this->m_FirstRoom.lock();
    const std::shared_ptr<BaseRoom> secondRoom = this->m_SecondRoom.lock();

    if (room == firstRoom) {
        return secondRoom;
    }
    if (room == secondRoom) {
        return firstRoom;
    }

    return nullptr;
}

GangwayOrientation Gangway::GetOrientation() const {
    return this->m_Orientation;
}

glm::vec2 Gangway::BuildAreaSize(const Config &config) {
    const float corridorLength = std::max(config.length, config.width);

    if (config.orientation == GangwayOrientation::Horizontal) {
        return {corridorLength, config.width};
    }

    return {config.width, corridorLength};
}

std::string Gangway::ResolveGangwaySprite(GangwayOrientation orientation) {
    if (orientation == GangwayOrientation::Horizontal) {
        return kHorizontalGangwaySprite;
    }

    return kVerticalGangwaySprite;
}

WallConfig Gangway::BuildWallConfig(const Config &config) {
    WallConfig wallConfig;
    wallConfig.top.thickness = config.wallThickness;
    wallConfig.right.thickness = config.wallThickness;
    wallConfig.bottom.thickness = config.wallThickness;
    wallConfig.left.thickness = config.wallThickness;

    if (config.orientation == GangwayOrientation::Horizontal) {
        wallConfig.left.hasOpening = true;
        wallConfig.left.openingSize = config.width;
        wallConfig.right.hasOpening = true;
        wallConfig.right.openingSize = config.width;
        return wallConfig;
    }

    wallConfig.top.hasOpening = true;
    wallConfig.top.openingSize = config.width;
    wallConfig.bottom.hasOpening = true;
    wallConfig.bottom.openingSize = config.width;
    return wallConfig;
}
