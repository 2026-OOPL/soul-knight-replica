#ifndef BASE_ROOM_WALL_LAYOUT_CONFIG_HPP
#define BASE_ROOM_WALL_LAYOUT_CONFIG_HPP

#include <algorithm>

#include "Common/Enums.hpp"

namespace BaseRoomWallLayoutConfig {

struct WallSideLayout {
    // 直接指定該面牆的最終厚度。
    // 設為 -1.0F 代表不覆蓋，沿用 BaseRoom 預設計算出的 thickness。
    float thicknessOverride = -1.0F;
    // 在目前 thickness 基礎上再加減的值。
    // 例如 -2.0F 代表變薄 2，3.0F 代表再加厚 3。
    float thicknessDelta = 0.0F;
    // 將整塊牆的碰撞箱沿著該面牆的法線方向平移。
    // top: 正值往上、bottom: 負值往下、right: 正值往右、left: 負值往左。
    // 這個值會同時影響 door 的對位，避免門和牆分離。
    float centerOffset = 0.0F;
    float openingOffsetDelta = 0.0F;
};

struct WallLayout {
    // 上牆設定
    WallSideLayout top;
    // 右牆設定
    WallSideLayout right;
    // 下牆設定
    WallSideLayout bottom;
    // 左牆設定
    WallSideLayout left;
};

inline WallSideLayout MakeWallSideLayout(
    // 直接覆蓋厚度；填 -1.0F 表示不覆蓋。
    float thicknessOverride = -1.0F,
    // 在目前厚度上加減。
    float thicknessDelta = 0.0F,
    // 推移牆的位置；方向依牆面不同而不同。
    float centerOffset = 0.0F,
    float openingOffsetDelta = 0.0F
) {
    return {
        thicknessOverride,
        thicknessDelta,
        centerOffset,
        openingOffsetDelta
    };
}

inline WallLayout MakeWallLayout(
    // 上牆設定
    const WallSideLayout &top = {},
    // 右牆設定
    const WallSideLayout &right = {},
    // 下牆設定
    const WallSideLayout &bottom = {},
    // 左牆設定
    const WallSideLayout &left = {}
) {
    return {
        top,
        right,
        bottom,
        left
    };
}

// 全域套用到所有 BaseRoom 的牆設定。
inline const WallLayout kGlobalWallLayout = MakeWallLayout(
    MakeWallSideLayout(-1.0F, 0.0F, -5.0F),
    MakeWallSideLayout(-1.0F, 5.0F, 0.0F, 5.0F),
    MakeWallSideLayout(-1.0F, 0.0F, 22.0F),
    MakeWallSideLayout(-1.0F, 5.0F, 0.0F, 5.0F)
);

// 依房間用途分類的牆設定。
inline const WallLayout kStarterWallLayout = MakeWallLayout();
inline const WallLayout kFightingWallLayout = MakeWallLayout();
inline const WallLayout kRewardWallLayout = MakeWallLayout();
inline const WallLayout kPortalWallLayout = MakeWallLayout();

// 依房間尺寸分類的牆設定。
inline const WallLayout kRoom13x13WallLayout = MakeWallLayout();
inline const WallLayout kRoom15x15WallLayout = MakeWallLayout();
inline const WallLayout kRoom17x17WallLayout = MakeWallLayout();
inline const WallLayout kRoom17x23WallLayout = MakeWallLayout();
inline const WallLayout kRoom23x17WallLayout = MakeWallLayout();

// 將單面牆設定套到實際的 WallSideConfig 上。
inline void ApplySideLayout(WallSideConfig &side, const WallSideLayout &layout) {
    if (layout.thicknessOverride >= 0.0F) {
        side.thickness = layout.thicknessOverride;
    }

    side.thickness = std::max(0.0F, side.thickness + layout.thicknessDelta);
    side.centerOffset += layout.centerOffset;
    side.openingOffset += layout.openingOffsetDelta;
}

// 一次套用四面牆設定。
inline void ApplyWallLayout(WallConfig &wallConfig, const WallLayout &layout) {
    ApplySideLayout(wallConfig.top, layout.top);
    ApplySideLayout(wallConfig.right, layout.right);
    ApplySideLayout(wallConfig.bottom, layout.bottom);
    ApplySideLayout(wallConfig.left, layout.left);
}

// 依房間用途選出對應的牆設定。
inline WallLayout ResolvePurposeWallLayout(RoomPurpose purpose) {
    switch (purpose) {
    case RoomPurpose::STARTER:
        return kStarterWallLayout;

    case RoomPurpose::FIGHTING:
        return kFightingWallLayout;

    case RoomPurpose::REWARD:
        return kRewardWallLayout;

    case RoomPurpose::PORTAL:
        return kPortalWallLayout;
    }

    return {};
}

// 依房間尺寸選出對應的牆設定。
inline WallLayout ResolveTypeWallLayout(RoomType roomType) {
    switch (roomType) {
    case RoomType::ROOM_13_13:
        return kRoom13x13WallLayout;

    case RoomType::ROOM_15_15:
        return kRoom15x15WallLayout;

    case RoomType::ROOM_17_17:
        return kRoom17x17WallLayout;

    case RoomType::ROOM_17_23:
        return kRoom17x23WallLayout;

    case RoomType::ROOM_23_17:
        return kRoom23x17WallLayout;
    }

    return {};
}

inline WallConfig ResolveWallConfig(
    // BaseRoom 原本根據 doorConfig 和 wallThickness 建出的牆設定。
    WallConfig wallConfig,
    // 房間用途，用來套用對應用途的牆設定。
    RoomPurpose purpose,
    // 房間尺寸，用來套用對應尺寸的牆設定。
    RoomType roomType
) {
    // 套用順序：
    // 1. 全域設定
    // 2. 房間用途設定
    // 3. 房間尺寸設定
    // 後面的設定可以覆蓋或追加前面的結果。
    ApplyWallLayout(wallConfig, kGlobalWallLayout);
    ApplyWallLayout(wallConfig, ResolvePurposeWallLayout(purpose));
    ApplyWallLayout(wallConfig, ResolveTypeWallLayout(roomType));
    return wallConfig;
}

} // namespace BaseRoomWallLayoutConfig

#endif
