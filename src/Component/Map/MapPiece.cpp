#include "Component/Map/MapPiece.hpp"
#include <glm/fwd.hpp>

bool MapPiece::GetVisibilityByCooridinate(glm::vec2 cooridinate) {
    auto context = Core::Context::GetInstance();

    glm::vec2 mapSize = this->GetScaledSize();
    glm::vec2 windowSize = {context->GetWindowWidth(), context->GetWindowHeight()};

    glm::vec2 mapPosition = GetPosition();
    glm::vec2 windowPosition = cooridinate;

    // Current window view port border in LTRB
    glm::vec4 windowBorder = {
        windowPosition.x - windowSize.x / 2,
        windowPosition.y + windowSize.y / 2,
        windowPosition.x + windowSize.x / 2,
        windowPosition.y - windowSize.y / 2,
    };

    // Current MapPiece render border in LTRB
    glm::vec4 mapBorder = {
        mapPosition.x - mapSize.x / 2,
        mapPosition.y + mapSize.y / 2,
        mapPosition.x + mapSize.x / 2,
        mapPosition.y - mapSize.y / 2,
    };

    const int margin = 10;
    
    float mLeft   = mapBorder.x - margin;
    float mTop    = mapBorder.y + margin;
    float mRight  = mapBorder.z + margin;
    float mBottom = mapBorder.w - margin;

    float wLeft   = windowBorder.x;
    float wTop    = windowBorder.y;
    float wRight  = windowBorder.z;
    float wBottom = windowBorder.w;

    bool isOutside = 
        (mRight  < wLeft  ) || // 整張在地圖左邊
        (mLeft   > wRight ) || // 整張在地圖右邊
        (mBottom > wTop   ) || // 整張在地圖上方 (Bottom 比 Window Top 還高)
        (mTop    < wBottom);   // 整張在地圖下方 (Top 比 Window Bottom 還低)

    return !isOutside;
}

void MapPiece::SetTransformByCooridinate(glm::vec2 cooridinate) {
    glm::vec2 scaledCooridinate = GetPosition();

    this->m_Transform.translation = scaledCooridinate - cooridinate;
}