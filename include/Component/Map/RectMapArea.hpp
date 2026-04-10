#ifndef RECT_MAP_AREA_HPP
#define RECT_MAP_AREA_HPP

#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Collision/IBlockingPrimitiveSource.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Map/MapPiece.hpp"

class RectMapArea : public MapPiece, public IBlockingPrimitiveSource {
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
    std::vector<Collision::CollisionPrimitive> CollectBlockingPrimitives(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const override;
    const std::vector<Collision::AxisAlignedBox> &GetStaticColliders() const;
    glm::vec2 GetAreaSize() const;
    glm::vec2 GetRenderSize() const;

protected:
    void SetAreaSize(const glm::vec2 &areaSize);
    void SetRenderSize(const glm::vec2 &renderSize);
    void SetWallConfig(const WallConfig &wallConfig);
    void RebuildStaticColliders();
    void SyncRenderSizeWithAreaSize();
    void UpdateRenderScale();

protected:
    glm::vec2 m_AreaSize = {0.0F, 0.0F};
    glm::vec2 m_RenderSize = {0.0F, 0.0F};
    bool m_ShouldSyncRenderSizeWithAreaSize = true;
    WallConfig m_WallConfig;
    std::vector<Collision::AxisAlignedBox> m_StaticColliders;
};

#endif
