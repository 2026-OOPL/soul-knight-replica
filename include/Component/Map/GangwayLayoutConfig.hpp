#ifndef GANGWAY_LAYOUT_CONFIG_HPP
#define GANGWAY_LAYOUT_CONFIG_HPP

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/MapColliderConfig.hpp"

namespace GangwayLayoutConfig {

struct LayoutProfile {
    glm::vec2 renderOffset = {0.0F, 0.0F};
    glm::vec2 renderSize = {0.0F, 0.0F};
    WallConfig wallConfig;
};

inline WallConfig MakeEmptyWallConfig() {
    WallConfig wallConfig;
    wallConfig.top.thickness = 0.0F;
    wallConfig.right.thickness = 0.0F;
    wallConfig.bottom.thickness = 0.0F;
    wallConfig.left.thickness = 0.0F;
    return wallConfig;
}

inline LayoutProfile MakeHorizontalProfile() {
    LayoutProfile profile;
    profile.wallConfig = MakeEmptyWallConfig();
    profile.renderOffset = {0.0F, -5.0F};
    profile.wallConfig.top.thickness = MapColliderConfig::kDefaultWallThickness;
    profile.wallConfig.top.centerOffset = 7.0F;
    profile.wallConfig.bottom.thickness = MapColliderConfig::kDefaultWallThickness;
    profile.wallConfig.bottom.centerOffset = 7.0F;
    return profile;
}

inline LayoutProfile MakeVerticalProfile() {
    LayoutProfile profile;
    profile.wallConfig = MakeEmptyWallConfig();
    profile.renderOffset = {0.0F, 0.0F};
    profile.wallConfig.right.thickness = MapColliderConfig::kDefaultWallThickness;
    profile.wallConfig.right.centerOffset = -1.0F;
    profile.wallConfig.left.thickness = MapColliderConfig::kDefaultWallThickness;
    profile.wallConfig.left.centerOffset = 1.0F;
    return profile;
}

inline const LayoutProfile kHorizontalProfile = MakeHorizontalProfile();
inline const LayoutProfile kVerticalProfile = MakeVerticalProfile();

inline const LayoutProfile &ResolveLayout(GangwayOrientation orientation) {
    switch (orientation) {
    case GangwayOrientation::Horizontal:
        return kHorizontalProfile;

    case GangwayOrientation::Vertical:
        return kVerticalProfile;
    }

    return kHorizontalProfile;
}

} // namespace GangwayLayoutConfig

#endif
