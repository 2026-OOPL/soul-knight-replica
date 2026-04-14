#ifndef GANGWAY_LAYOUT_CONFIG_HPP
#define GANGWAY_LAYOUT_CONFIG_HPP

#include <glm/vec2.hpp>

namespace GangwayLayoutConfig {

inline const glm::vec2 kHorizontalPositionOffset = {0.0F, 0.0F};
inline const glm::vec2 kVerticalPositionOffset = {0.0F, 0.0F};
inline constexpr float kHorizontalTopWallOffset = 7.0F;
inline constexpr float kHorizontalRightWallOffset = 0.0F;
inline constexpr float kHorizontalBottomWallOffset = 7.0F;
inline constexpr float kHorizontalLeftWallOffset = 0.0F;
inline constexpr float kVerticalTopWallOffset = 0.0F;
inline constexpr float kVerticalRightWallOffset =-7.0F;
inline constexpr float kVerticalBottomWallOffset = 0.0F;
inline constexpr float kVerticalLeftWallOffset = 7.0F;

} // namespace GangwayLayoutConfig

#endif
