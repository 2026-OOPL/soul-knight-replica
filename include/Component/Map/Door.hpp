#ifndef MAP_DOOR_HPP
#define MAP_DOOR_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Collision/ICollidable.hpp"
#include "Component/Collision/IBlockingPrimitiveSource.hpp"
#include "Component/IStateful.hpp"
#include "Component/Map/MapPiece.hpp"

namespace Core {
class Drawable;
}

namespace Util {
class Animation;
}

enum class DoorSide {
    Top,
    Right,
    Bottom,
    Left
};

class Door : public MapPiece,
             public IStateful,
             public IBlockingPrimitiveSource,
             public ICollidable {
public:
    enum class State {
        Closed,
        OpenDelay,
        Opening,
        Open,
        Closing
    };

    struct Visuals {
        std::shared_ptr<Core::Drawable> closedIdle;
        std::shared_ptr<Core::Drawable> openIdle;
        std::shared_ptr<Util::Animation> opening = nullptr;
        std::shared_ptr<Util::Animation> closing = nullptr;
    };

    Door(
        glm::vec2 cooridinate,
        DoorSide side,
        glm::vec2 colliderSize,
        glm::vec2 renderSize,
        Visuals visuals,
        bool isOpen = true
    );

    void Update() override;
    std::vector<Collision::CollisionPrimitive> CollectBlockingPrimitives(
        const Collision::AxisAlignedBox *ignoreOverlapBox = nullptr
    ) const override;
    glm::vec2 GetCollisionOrigin() const override;
    const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const override;
    void OnCollision(const Collision::CollisionSituation &situation) override;

    glm::vec2 GetAbsoluteScale() const override;

    void Open();
    void Close();
    void Toggle();

    bool IsOpen() const; 
    State GetState() const; 
    DoorSide GetSide() const; 

private:
    void ApplyDrawable(const std::shared_ptr<Core::Drawable> &drawable);
    void EnterState(State state);

    Visuals m_Visuals;
    DoorSide m_Side;
    State m_State = State::Closed;
    glm::vec2 m_RenderSize = {1.0F, 1.0F};
    float m_OpenDelayRemainingMs = 0.0F;
    float m_BaseRotation = 0.0F;
    std::vector<Collision::CollisionBox> m_CollisionBoxes;
};

#endif
