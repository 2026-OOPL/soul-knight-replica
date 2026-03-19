#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP

#include <functional>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

class MapPiece;

namespace Collision {

struct AxisAlignedBox { //定義碰撞箱資料
    glm::vec2 center = {0.0F, 0.0F}; //中心點
    glm::vec2 size = {0.0F, 0.0F}; //大小
};

struct MovementResult { // 對被擋住的可移動物件做出調整
    glm::vec2 resolvedDelta = {0.0F, 0.0F}; // 被擋住後要怎麼移動
    bool blockedX = false; //x 軸移動時有沒有被擋住
    bool blockedY = false; //y 軸移動時有沒有被擋住
};

class CollisionSystem {
public:
    using BlockingBoxProvider = std::function<std::vector<AxisAlignedBox>()>; //設定類別的別稱

    static AxisAlignedBox BuildBox(const glm::vec2 &center, const glm::vec2 &size); //輸入中心、大小，輸出碰撞箱

    void SetStaticBlockingBoxes(const std::vector<AxisAlignedBox> &blockingBoxes); //把一組固定不會變的阻擋碰撞箱設定到 CollisionSystem 裡
    void SetBlockingBoxProvider(BlockingBoxProvider blockingBoxProvider); // 把一個能提供阻擋碰撞箱列表的函式交給 CollisionSystem 保存起來

    std::vector<AxisAlignedBox> GetBlockingBoxes() const; //回傳目前地圖上已存在的碰撞箱

    bool IsOverlapping(const AxisAlignedBox &lhs, const AxisAlignedBox &rhs) const; //判斷兩物件是否重疊
    bool IsBlocked(const AxisAlignedBox &box) const; //判斷此物件是否有和系統上任何物件重疊

    MovementResult ResolveMovement( //計算碰撞後的實際位移量
        const AxisAlignedBox &currentBox, //物件現在所在位置的碰撞箱
        const glm::vec2 &intendedDelta //這一幀原本想移動的位移量
    ) const;

private:
    std::vector<AxisAlignedBox> m_StaticBlockingBoxes; //用來保存靜態阻擋碰撞箱的陣列
    BlockingBoxProvider m_BlockingBoxProvider; //一個函式物件，用來在需要時提供阻擋碰撞箱
};

std::vector<AxisAlignedBox> BuildWallBoxes( //建立牆碰撞箱列表
    const std::vector<std::shared_ptr<MapPiece>> &pieces
);

std::vector<AxisAlignedBox> BuildRoomBoundaryBoxes( //根據房間中心與大小建立ㄧ個正方形房間
    const glm::vec2 &roomCenter,
    const glm::vec2 &roomSize,
    float wallThickness
);

} // namespace Collision

#endif
