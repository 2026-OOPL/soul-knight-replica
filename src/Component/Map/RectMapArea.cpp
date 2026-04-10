#include <algorithm>
#include <cmath>

#include "Component/Map/RectMapArea.hpp"

namespace {

glm::vec2 SafeScaleForSize(
    const std::shared_ptr<Core::Drawable> &drawable,
    const glm::vec2 &areaSize
) {
    if (drawable == nullptr) {
        return {1.0F, 1.0F};
    }

    const glm::vec2 drawableSize = drawable->GetSize();
    return {
        areaSize.x / std::max(drawableSize.x, 1.0F),
        areaSize.y / std::max(drawableSize.y, 1.0F)
    };
}

std::vector<Collision::AxisAlignedBox> BuildHorizontalWallSegments(
    const glm::vec2 &areaCenter,
    float wallY,
    float wallWidth,
    float wallThickness,
    const Collision::WallOpening &opening
) {
    if (wallWidth <= 0.0F || wallThickness <= 0.0F) {
        return {};
    }

    const float openingWidth = std::clamp(opening.size, 0.0F, wallWidth);
    if (openingWidth <= 0.0F) {
        return {
            Collision::CollisionSystem::BuildBox(
                {areaCenter.x, wallY},
                {wallWidth, wallThickness}
            )
        };
    }

    const float wallLeft = areaCenter.x - wallWidth / 2.0F;
    const float wallRight = areaCenter.x + wallWidth / 2.0F;
    const float openingHalfWidth = openingWidth / 2.0F;
    const float openingCenterX = std::clamp(
        areaCenter.x + opening.offset,
        wallLeft + openingHalfWidth,
        wallRight - openingHalfWidth
    );
    const float leftWidth = std::max(0.0F, openingCenterX - openingHalfWidth - wallLeft);
    const float rightWidth = std::max(0.0F, wallRight - openingCenterX - openingHalfWidth);
    std::vector<Collision::AxisAlignedBox> segments;

    if (leftWidth > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallLeft + leftWidth / 2.0F, wallY},
            {leftWidth, wallThickness}
        ));
    }

    if (rightWidth > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallRight - rightWidth / 2.0F, wallY},
            {rightWidth, wallThickness}
        ));
    }

    return segments;
}

std::vector<Collision::AxisAlignedBox> BuildVerticalWallSegments(
    const glm::vec2 &areaCenter,
    float wallX,
    float wallHeight,
    float wallThickness,
    const Collision::WallOpening &opening
) {
    if (wallHeight <= 0.0F || wallThickness <= 0.0F) {
        return {};
    }

    const float openingHeight = std::clamp(opening.size, 0.0F, wallHeight);
    if (openingHeight <= 0.0F) {
        return {
            Collision::CollisionSystem::BuildBox(
                {wallX, areaCenter.y},
                {wallThickness, wallHeight}
            )
        };
    }

    const float wallBottom = areaCenter.y - wallHeight / 2.0F;
    const float wallTop = areaCenter.y + wallHeight / 2.0F;
    const float openingHalfHeight = openingHeight / 2.0F;
    const float openingCenterY = std::clamp(
        areaCenter.y + opening.offset,
        wallBottom + openingHalfHeight,
        wallTop - openingHalfHeight
    );
    const float bottomHeight =
        std::max(0.0F, openingCenterY - openingHalfHeight - wallBottom);
    const float topHeight = std::max(0.0F, wallTop - openingCenterY - openingHalfHeight);
    std::vector<Collision::AxisAlignedBox> segments;

    if (bottomHeight > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallX, wallBottom + bottomHeight / 2.0F},
            {wallThickness, bottomHeight}
        ));
    }

    if (topHeight > 0.0F) {
        segments.push_back(Collision::CollisionSystem::BuildBox(
            {wallX, wallTop - topHeight / 2.0F},
            {wallThickness, topHeight}
        ));
    }

    return segments;
}

Collision::WallOpening BuildOpening(const WallSideConfig &sideConfig) {
    if (!sideConfig.hasOpening) {
        return {};
    }

    return {
        sideConfig.openingSize,
        sideConfig.openingOffset
    };
}

} // namespace

RectMapArea::RectMapArea(
    const glm::vec2 &absolutePosition,
    const std::shared_ptr<Core::Drawable> &drawable,
    const glm::vec2 &areaSize,
    const WallConfig &wallConfig
)
    : MapPiece(absolutePosition, drawable),
      m_AreaSize(areaSize),
      m_RenderSize(areaSize),
      m_WallConfig(wallConfig) {
    this->UpdateRenderScale();
    this->SetColliderSize(this->m_AreaSize);
    this->RebuildStaticColliders();
}

RectMapArea::RectMapArea(
    const glm::vec2 &absolutePosition,
    const std::string &resource,
    const glm::vec2 &areaSize,
    const WallConfig &wallConfig
)
    : RectMapArea(
          absolutePosition,
          std::make_shared<Util::Image>(resource, false),
          areaSize,
          wallConfig
      ) {
}

bool RectMapArea::IsPointInside(const glm::vec2 &point) const {
    const glm::vec2 halfSize = this->m_AreaSize / 2.0F;
    const glm::vec2 offset = point - this->GetAbsoluteTranslation();

    return std::abs(offset.x) <= halfSize.x &&
           std::abs(offset.y) <= halfSize.y;
}

std::vector<Collision::CollisionPrimitive> RectMapArea::CollectBlockingPrimitives(
    const Collision::AxisAlignedBox *ignoreOverlapBox
) const {
    (void)ignoreOverlapBox;
    return Collision::BuildStaticWorldPrimitives(this->m_StaticColliders);
}

const std::vector<Collision::AxisAlignedBox> &RectMapArea::GetStaticColliders() const {
    return this->m_StaticColliders;
}

glm::vec2 RectMapArea::GetAreaSize() const {
    return this->m_AreaSize;
}

glm::vec2 RectMapArea::GetRenderSize() const {
    return this->m_RenderSize;
}

void RectMapArea::SetAreaSize(const glm::vec2 &areaSize) {
    this->m_AreaSize = areaSize;
    this->SetColliderSize(areaSize);

    if (this->m_ShouldSyncRenderSizeWithAreaSize) {
        this->m_RenderSize = areaSize;
    }

    this->UpdateRenderScale();
    this->RebuildStaticColliders();
}

void RectMapArea::SetRenderSize(const glm::vec2 &renderSize) {
    this->m_RenderSize = renderSize;
    this->m_ShouldSyncRenderSizeWithAreaSize = false;
    this->UpdateRenderScale();
}

void RectMapArea::SetWallConfig(const WallConfig &wallConfig) {
    this->m_WallConfig = wallConfig;
    this->RebuildStaticColliders();
}

void RectMapArea::SyncRenderSizeWithAreaSize() {
    this->m_ShouldSyncRenderSizeWithAreaSize = true;
    this->m_RenderSize = this->m_AreaSize;
    this->UpdateRenderScale();
}

void RectMapArea::UpdateRenderScale() {
    this->m_AbsoluteTransform.scale = SafeScaleForSize(this->m_Drawable, this->m_RenderSize);
}

void RectMapArea::RebuildStaticColliders() {
    this->m_StaticColliders.clear();

    const glm::vec2 areaCenter = this->GetAbsoluteTranslation();
    const glm::vec2 areaHalfSize = this->m_AreaSize / 2.0F;

    const std::vector<Collision::AxisAlignedBox> topWalls = BuildHorizontalWallSegments(
        areaCenter,
        areaCenter.y + areaHalfSize.y - this->m_WallConfig.top.thickness / 2.0F +
            this->m_WallConfig.top.centerOffset,
        this->m_AreaSize.x,
        this->m_WallConfig.top.thickness,
        BuildOpening(this->m_WallConfig.top)
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        topWalls.begin(),
        topWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> rightWalls = BuildVerticalWallSegments(
        areaCenter,
        areaCenter.x + areaHalfSize.x - this->m_WallConfig.right.thickness / 2.0F +
            this->m_WallConfig.right.centerOffset,
        this->m_AreaSize.y,
        this->m_WallConfig.right.thickness,
        BuildOpening(this->m_WallConfig.right)
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        rightWalls.begin(),
        rightWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> bottomWalls = BuildHorizontalWallSegments(
        areaCenter,
        areaCenter.y - areaHalfSize.y + this->m_WallConfig.bottom.thickness / 2.0F +
            this->m_WallConfig.bottom.centerOffset,
        this->m_AreaSize.x,
        this->m_WallConfig.bottom.thickness,
        BuildOpening(this->m_WallConfig.bottom)
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        bottomWalls.begin(),
        bottomWalls.end()
    );

    const std::vector<Collision::AxisAlignedBox> leftWalls = BuildVerticalWallSegments(
        areaCenter,
        areaCenter.x - areaHalfSize.x + this->m_WallConfig.left.thickness / 2.0F +
            this->m_WallConfig.left.centerOffset,
        this->m_AreaSize.y,
        this->m_WallConfig.left.thickness,
        BuildOpening(this->m_WallConfig.left)
    );
    this->m_StaticColliders.insert(
        this->m_StaticColliders.end(),
        leftWalls.begin(),
        leftWalls.end()
    );
}
