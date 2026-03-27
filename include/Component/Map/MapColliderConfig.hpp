#ifndef MAP_COLLIDER_CONFIG_HPP
#define MAP_COLLIDER_CONFIG_HPP

#include <glm/vec2.hpp>

namespace MapColliderConfig {

inline constexpr float kDefaultWallThickness = 20.0F;
inline const glm::vec2 kHorizontalDoorColliderSize = {80.0F, 15.0F};//y軸為門的碰撞厚度
inline const glm::vec2 kHorizontalDoorRenderSize = {80.0F, 32.0F};
inline const glm::vec2 kVerticalDoorColliderSize = {15.0F, 80.0F};//x軸為門的碰撞厚度
inline const glm::vec2 kVerticalDoorRenderSize = {16.0F, 96.0F};

} // namespace MapColliderConfig

#endif
