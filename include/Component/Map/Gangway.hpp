#ifndef GANGWAY_HPP
#define GANGWAY_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Map/RectMapArea.hpp"

class BaseRoom;

enum class GangwayOrientation {
    Horizontal,
    Vertical
};

class Gangway : public RectMapArea {
public:
    struct Config {
        GangwayOrientation orientation = GangwayOrientation::Horizontal;
        float length = 0.0F;
        float width = 80.0F;
        float wallThickness = 10.0F;
        glm::vec2 positionOffset = {0.0F, 0.0F};
        glm::vec2 renderSize = {0.0F, 0.0F};
    };

    Gangway(const glm::vec2 &absolutePosition, Config config);

    void ConnectRooms(
        const std::shared_ptr<BaseRoom> &firstRoom,
        const std::shared_ptr<BaseRoom> &secondRoom
    );

    bool ConnectsRoom(const std::shared_ptr<BaseRoom> &room) const;
    std::shared_ptr<BaseRoom> GetOtherRoom(const std::shared_ptr<BaseRoom> &room) const;
    GangwayOrientation GetOrientation() const;

private:
    static glm::vec2 BuildAreaSize(const Config &config);
    static std::string ResolveGangwaySprite(GangwayOrientation orientation);
    static WallConfig BuildWallConfig(const Config &config);

    GangwayOrientation m_Orientation = GangwayOrientation::Horizontal;
    std::weak_ptr<BaseRoom> m_FirstRoom;
    std::weak_ptr<BaseRoom> m_SecondRoom;
};

#endif
