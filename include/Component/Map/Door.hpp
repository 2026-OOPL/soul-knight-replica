#ifndef MAP_DOOR_HPP
#define MAP_DOOR_HPP

#include <glm/vec2.hpp>
#include <memory>

#include "Component/IStateful.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Util/Text.hpp"

enum class DoorSide {
    Top,
    Right,
    Bottom,
    Left
};

class Door : public MapPiece, public IStateful {
public:
    Door(
        glm::vec2 cooridinate,
        DoorSide side,
        glm::vec2 colliderSize,
        bool isOpen = true
    );

    void Update() override;

    void Open();
    void Close();
    void Toggle();

    bool IsOpen() const;
    bool IsClosed() const;
    DoorSide GetSide() const;

private:
    glm::vec2 MoveTowards(
        const glm::vec2 &current,
        const glm::vec2 &target,
        float maxDistanceDelta
    ) const;

    DoorSide m_Side;
    std::shared_ptr<Util::Text> m_DrawableText;
    glm::vec2 m_ClosedScale = {1.0F, 1.0F};
    glm::vec2 m_OpenScale = {1.0F, 0.15F};
    float m_BaseRotation = 0.0F;
    bool m_IsOpen = true;
};

#endif
