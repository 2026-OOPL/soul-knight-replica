#ifndef MAP_DOOR_HPP
#define MAP_DOOR_HPP

#include <glm/vec2.hpp>
#include <memory>

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

class Door : public MapPiece, public IStateful {
public:
    enum class State {
        Closed,
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

    glm::vec2 GetObjectSize() override;

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
    float m_BaseRotation = 0.0F;
};

#endif
