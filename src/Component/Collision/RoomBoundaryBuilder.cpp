#include <algorithm>
#include <vector>

#include "Component/Collision/RoomBoundaryBuilder.hpp"

namespace {

Collision::AxisAlignedBox BuildBox(const glm::vec2 &center, const glm::vec2 &size) {
    return {
        center,
        size
    };
}

std::vector<Collision::AxisAlignedBox> BuildHorizontalWallSegments(
    const glm::vec2 &areaCenter,
    float wallY,
    float wallWidth,
    float thickness,
    const Collision::WallOpening &opening
) {
    if (wallWidth <= 0.0F || thickness <= 0.0F) {
        return {};
    }

    const float openingWidth = std::clamp(opening.size, 0.0F, wallWidth);
    if (openingWidth <= 0.0F) {
        return {
            BuildBox(
                {areaCenter.x, wallY},
                {wallWidth, thickness}
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
        segments.push_back(BuildBox(
            {wallLeft + leftWidth / 2.0F, wallY},
            {leftWidth, thickness}
        ));
    }

    if (rightWidth > 0.0F) {
        segments.push_back(BuildBox(
            {wallRight - rightWidth / 2.0F, wallY},
            {rightWidth, thickness}
        ));
    }

    return segments;
}

std::vector<Collision::AxisAlignedBox> BuildVerticalWallSegments(
    const glm::vec2 &areaCenter,
    float wallX,
    float wallHeight,
    float thickness,
    const Collision::WallOpening &opening
) {
    if (wallHeight <= 0.0F || thickness <= 0.0F) {
        return {};
    }

    const float openingHeight = std::clamp(opening.size, 0.0F, wallHeight);
    if (openingHeight <= 0.0F) {
        return {
            BuildBox(
                {wallX, areaCenter.y},
                {thickness, wallHeight}
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
    const float topHeight =
        std::max(0.0F, wallTop - openingCenterY - openingHalfHeight);
    std::vector<Collision::AxisAlignedBox> segments;

    if (bottomHeight > 0.0F) {
        segments.push_back(BuildBox(
            {wallX, wallBottom + bottomHeight / 2.0F},
            {thickness, bottomHeight}
        ));
    }

    if (topHeight > 0.0F) {
        segments.push_back(BuildBox(
            {wallX, wallTop - topHeight / 2.0F},
            {thickness, topHeight}
        ));
    }

    return segments;
}

} // namespace

namespace Collision {

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness
) {
    return BuildRoomBoundaryBoxes(
        roomCenter,
        roomSize,
        wallThickness,
        RoomBoundaryOpenings{}
    );
}

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes(
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness,
    const RoomBoundaryOpenings &openings
) {
    const float safeWallThickness = std::max(0.0F, wallThickness);
    const glm::vec2 roomHalfSize = roomSize / 2.0F;
    const float halfWallThickness = safeWallThickness / 2.0F;
    std::vector<AxisAlignedBox> boundaryBoxes;

    const std::vector<AxisAlignedBox> topWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y + roomHalfSize.y - halfWallThickness,
        roomSize.x,
        safeWallThickness,
        openings.top
    );
    boundaryBoxes.insert(boundaryBoxes.end(), topWalls.begin(), topWalls.end());

    const std::vector<AxisAlignedBox> bottomWalls = BuildHorizontalWallSegments(
        roomCenter,
        roomCenter.y - roomHalfSize.y + halfWallThickness + 20.0F,
        roomSize.x,
        safeWallThickness,
        openings.bottom
    );
    boundaryBoxes.insert(boundaryBoxes.end(), bottomWalls.begin(), bottomWalls.end());

    const std::vector<AxisAlignedBox> leftWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x - roomHalfSize.x + halfWallThickness,
        roomSize.y,
        safeWallThickness,
        openings.left
    );
    boundaryBoxes.insert(boundaryBoxes.end(), leftWalls.begin(), leftWalls.end());

    const std::vector<AxisAlignedBox> rightWalls = BuildVerticalWallSegments(
        roomCenter,
        roomCenter.x + roomHalfSize.x - halfWallThickness,
        roomSize.y,
        safeWallThickness,
        openings.right
    );
    boundaryBoxes.insert(boundaryBoxes.end(), rightWalls.begin(), rightWalls.end());

    return boundaryBoxes;
}

} // namespace Collision
