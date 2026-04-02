#ifndef GANGWAY_LAYOUT_CONFIG_HPP
#define GANGWAY_LAYOUT_CONFIG_HPP

#include <glm/vec2.hpp>

namespace GangwayLayoutConfig {

inline const glm::vec2 kHorizontalPositionOffset = {8.0F, -8.0F};
inline const glm::vec2 kVerticalPositionOffset = {0.0F, 4.0F};
inline constexpr float kHorizontalTopWallOffset = 20.0F;
inline constexpr float kHorizontalRightWallOffset = 0.0F;
inline constexpr float kHorizontalBottomWallOffset = 15.0F;
inline constexpr float kHorizontalLeftWallOffset = 0.0F;
inline constexpr float kVerticalTopWallOffset = 0.0F;
inline constexpr float kVerticalRightWallOffset = -10.0F;
inline constexpr float kVerticalBottomWallOffset = 0.0F;
inline constexpr float kVerticalLeftWallOffset = 10.0F;

} // namespace GangwayLayoutConfig

#endif
