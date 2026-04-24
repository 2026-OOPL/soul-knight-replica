#include <algorithm>
#include <memory>
#include <utility>

#include "Component/Prop/IndestructibleObstacle.hpp"

#include "Core/Drawable.hpp"

IndestructibleObstacle::IndestructibleObstacle(
    const glm::vec2 &coordinate,
    Config config
) {
    this->SetAbsoluteTranslation(coordinate);
    this->SetOwningRoom(std::move(config.owningRoom));
    this->SetBlockingSize(config.blockingSize);
    this->SetBlockingOffset(config.blockingOffset);
    this->SetZIndex(static_cast<float>(config.zIndex));
    this->SetDrawable(config.visual);
    this->SetAbsoluteScale(SafeScaleForSize(config.visual, config.renderSize));
}

void IndestructibleObstacle::Update() {
}

bool IndestructibleObstacle::IsDebugVisible() const {
    return true;
}

glm::vec2 IndestructibleObstacle::SafeScaleForSize(
    const std::shared_ptr<Core::Drawable> &drawable,
    const glm::vec2 &renderSize
) {
    if (drawable == nullptr) {
        return {1.0F, 1.0F};
    }

    const glm::vec2 drawableSize = drawable->GetSize();
    return {
        renderSize.x / std::max(drawableSize.x, 1.0F),
        renderSize.y / std::max(drawableSize.y, 1.0F)
    };
}
