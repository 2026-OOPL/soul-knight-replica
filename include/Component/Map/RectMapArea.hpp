#ifndef RECT_MAP_AREA_HPP
#define RECT_MAP_AREA_HPP

#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/MapPiece.hpp"

class RectMapArea : public MapPiece {
public:
    RectMapArea(
        const glm::vec2 &absolutePosition,
        const std::shared_ptr<Core::Drawable> &drawable,
        const glm::vec2 &areaSize,
        const WallConfig &wallConfig
    );

    RectMapArea(
        const glm::vec2 &absolutePosition,
        const std::string &resource,
        const glm::vec2 &areaSize,
        const WallConfig &wallConfig
    );

    bool IsPointInside(const glm::vec2 &point) const;
    const std::vector<Collision::AxisAlignedBox> &GetStaticColliders() const;
    glm::vec2 GetAreaSize() const;

protected:
    void SetAreaSize(const glm::vec2 &areaSize);
    void SetWallConfig(const WallConfig &wallConfig);
    void RebuildStaticColliders();

protected:
    glm::vec2 m_AreaSize = {0.0F, 0.0F};
    WallConfig m_WallConfig;
    std::vector<Collision::AxisAlignedBox> m_StaticColliders;
};

#endif
