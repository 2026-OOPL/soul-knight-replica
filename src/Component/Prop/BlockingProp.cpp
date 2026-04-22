#include <algorithm>

#include "Component/Prop/BlockingProp.hpp"
#include "Component/Collision/CollisionSystem.hpp"

BlockingProp::BlockingProp() = default;

std::vector<Collision::CollisionPrimitive> BlockingProp::CollectBlockingPrimitives(
    const Collision::AxisAlignedBox *ignoreOverlapBox
) const {
    const Collision::AxisAlignedBox box = this->BuildBlockingBox();
    if (box.size.x <= 0.0F || box.size.y <= 0.0F) {
        return {};
    }

    if (ignoreOverlapBox != nullptr) {
        Collision::CollisionSystem collisionSystem;
        if (collisionSystem.IsOverlapping(box, *ignoreOverlapBox)) {
            return {};
        }
    }

    return Collision::BuildStaticWorldPrimitives({box});
}

void BlockingProp::SetOwningRoom(const std::shared_ptr<BaseRoom> &room) {
    this->m_OwningRoom = room;
}

std::shared_ptr<BaseRoom> BlockingProp::GetOwningRoom() const {
    return this->m_OwningRoom.lock();
}

bool BlockingProp::BelongsToRoom(const std::shared_ptr<BaseRoom> &room) const {
    const std::shared_ptr<BaseRoom> owningRoom = this->m_OwningRoom.lock();
    return owningRoom == nullptr || owningRoom == room;
}

glm::vec2 BlockingProp::GetBlockingSize() const {
    return this->m_BlockingSize;
}

void BlockingProp::SetBlockingSize(const glm::vec2 &blockingSize) {
    this->m_BlockingSize = {
        std::max(0.0F, blockingSize.x),
        std::max(0.0F, blockingSize.y)
    };
}

glm::vec2 BlockingProp::GetBlockingOffset() const {
    return this->m_BlockingOffset;
}

void BlockingProp::SetBlockingOffset(const glm::vec2 &blockingOffset) {
    this->m_BlockingOffset = blockingOffset;
}

Collision::AxisAlignedBox BlockingProp::BuildBlockingBox() const {
    return Collision::CollisionSystem::BuildBox(
        this->GetAbsoluteTranslation() + this->m_BlockingOffset,
        this->m_BlockingSize
    );
}
