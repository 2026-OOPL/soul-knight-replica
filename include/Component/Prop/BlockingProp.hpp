#ifndef BLOCKING_PROP_HPP
#define BLOCKING_PROP_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/IBlockingPrimitiveSource.hpp"
#include "Component/Prop/Prop.hpp"

class BaseRoom;

class BlockingProp : public Prop, public IBlockingPrimitiveSource {
public:
    BlockingProp();
    ~BlockingProp() override = default;

    std::vector<Collision::CollisionPrimitive> CollectBlockingPrimitives(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const override;

    void SetOwningRoom(const std::shared_ptr<BaseRoom> &room);
    std::shared_ptr<BaseRoom> GetOwningRoom() const;
    bool BelongsToRoom(const std::shared_ptr<BaseRoom> &room) const;

    glm::vec2 GetBlockingSize() const;
    void SetBlockingSize(const glm::vec2 &blockingSize);
    glm::vec2 GetBlockingOffset() const;
    void SetBlockingOffset(const glm::vec2 &blockingOffset);

protected:
    Collision::AxisAlignedBox BuildBlockingBox() const;

private:
    std::weak_ptr<BaseRoom> m_OwningRoom;
    glm::vec2 m_BlockingSize = {0.0F, 0.0F};
    glm::vec2 m_BlockingOffset = {0.0F, 0.0F};
};

#endif
