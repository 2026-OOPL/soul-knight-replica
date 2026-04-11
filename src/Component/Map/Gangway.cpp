#include <algorithm>

#include "Component/Map/Gangway.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/GangwayLayoutConfig.hpp"

namespace {

constexpr char kHorizontalGangwaySprite[] = RESOURCE_DIR "/Map/Gangway/Gangway_15x5.png";
constexpr char kVerticalGangwaySprite[] = RESOURCE_DIR "/Map/Gangway/Gangway_5x15.png";

float ClampThickness(float thickness) {
    return std::max(0.0F, thickness);
}

WallConfig BuildGangwayWallConfig(const Gangway::Config &config) {
    WallConfig wallConfig;
    wallConfig.top.thickness = 0.0F;
    wallConfig.right.thickness = 0.0F;
    wallConfig.bottom.thickness = 0.0F;
    wallConfig.left.thickness = 0.0F;

    if (config.orientation == GangwayOrientation::Horizontal) {
        wallConfig.top.thickness = ClampThickness(config.wallThickness);
        wallConfig.bottom.thickness = ClampThickness(config.wallThickness);
        wallConfig.top.centerOffset = config.topWallOffset;
        wallConfig.bottom.centerOffset = config.bottomWallOffset;
        return wallConfig;
    }

    wallConfig.right.thickness = ClampThickness(config.wallThickness);
    wallConfig.left.thickness = ClampThickness(config.wallThickness);
    wallConfig.right.centerOffset = config.rightWallOffset;
    wallConfig.left.centerOffset = config.leftWallOffset;
    return wallConfig;
}

glm::vec2 BuildGangwayAreaSize(const Gangway::Config &config) {
    const WallConfig wallConfig = BuildGangwayWallConfig(config);
    const float corridorLength = std::max(config.length, config.width);

    // `width` models the clear walkable corridor width. Wall thickness is
    // added outside that corridor so the playable passage stays unchanged.
    if (config.orientation == GangwayOrientation::Horizontal) {
        return {
            corridorLength,
            config.width + wallConfig.top.thickness + wallConfig.bottom.thickness
        };
    }

    return {
        config.width + wallConfig.left.thickness + wallConfig.right.thickness,
        corridorLength
    };
}

glm::vec2 ResolveRenderSize(
    const Gangway::Config &config,
    const std::shared_ptr<Core::Drawable> &drawable
) {
    if (config.renderSize.x > 0.0F && config.renderSize.y > 0.0F) {
        return config.renderSize;
    }

    if (drawable != nullptr) {
        return drawable->GetSize();
    }

    return BuildGangwayAreaSize(config);
}

} // namespace

Gangway::Gangway(const glm::vec2 &absolutePosition, Config config)
    : RectMapArea(
          absolutePosition + config.positionOffset,
          Gangway::ResolveGangwaySprite(config.orientation),
          Gangway::BuildAreaSize(config),
          Gangway::BuildWallConfig(config)
      ),
      m_Orientation(config.orientation) {
    this->SetRenderSize(ResolveRenderSize(config, this->m_Drawable));
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
    return BuildGangwayAreaSize(config);
}

std::string Gangway::ResolveGangwaySprite(GangwayOrientation orientation) {
    if (orientation == GangwayOrientation::Horizontal) {
        return kHorizontalGangwaySprite;
    }

    return kVerticalGangwaySprite;
}

WallConfig Gangway::BuildWallConfig(const Config &config) {
    return BuildGangwayWallConfig(config);
}
