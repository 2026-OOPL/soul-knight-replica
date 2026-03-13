#include "Map/MapPiece.hpp"

void MapPiece::ToggleVisibilityByCooridinate(glm::vec2 cooridinate) {
    auto context = Core::Context::GetInstance();

    glm::vec2 itemSize = this->GetScaledSize();
    glm::vec2 windowSize = {context->GetWindowWidth(), context->GetWindowHeight()};

    glm::vec2 itemPosition = this->cooridinate;
    glm::vec2 windowPosition = cooridinate;

    // Current window view port border in LTRB
    glm::vec4 windowBorder = {
        windowPosition.x - windowSize.x / 2,
        windowPosition.y + windowSize.y / 2,
        windowPosition.x + windowSize.x / 2,
        windowPosition.y - windowSize.y / 2,
    };

    // Current MapPiece render border in LTRB
    glm::vec4 itemBorder = {
        itemPosition.x - itemSize.x / 2,
        itemPosition.y + itemSize.y / 2,
        itemPosition.x + itemSize.x / 2,
        itemPosition.y - itemSize.y / 2,
    };
    
    bool needRender = !(
        // Map right border is smaller than window left border
        itemBorder.z < windowBorder.x ||
        // Map left border is greater than window right border
        itemBorder.x > windowBorder.z

        // Map

    );
    
}