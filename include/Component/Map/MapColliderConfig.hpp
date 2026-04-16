#ifndef MAP_COLLIDER_CONFIG_HPP
#define MAP_COLLIDER_CONFIG_HPP

#include <glm/vec2.hpp>

namespace MapColliderConfig {

inline constexpr float kDefaultWallThickness = 10.0F;
inline constexpr float kHorizontalDoorOpeningWidth = 80.0F;
inline const glm::vec2 kHorizontalDoorRenderSize = {80.0F, 32.0F};
inline constexpr float kVerticalDoorOpeningHeight = 100.0F;
inline const glm::vec2 kVerticalDoorRenderSize = {16.0F, 96.0F};

} // namespace MapColliderConfig

#endif
