#ifndef DOOR_1x5_HPP
#define DOOR_1x5_HPP

#include <glm/vec2.hpp>

#include "Common/Constants.hpp"
#include "Component/ICollidiable.hpp"
#include "Component/IMapObject.hpp"
#include "Component/Map/MapPiece.hpp"


class Door1x5 : public MapPiece, public ICollidable {

public:
    Door1x5(glm::vec2 cooridinate);
    
    glm::vec2 GetColliderSize() const override;
    void SetColliderSize(const glm::vec2 &colliderSize) override;
    
    glm::vec2 GetColliderCooridinate() const override;

private:

    bool m_IsOpen = false;

    glm::vec2 m_ColliderSize = {
        MAP_PIXEL_PER_BLOCK * 1,
        MAP_PIXEL_PER_BLOCK * 5
    };
};

#endif // WALL_1x5_HPP