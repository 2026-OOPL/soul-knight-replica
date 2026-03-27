#ifndef BASE_ROOM_HPP
#define BASE_ROOM_HPP

#include <glm/vec2.hpp>

#include "Common/Enums.hpp"
#include "Component/Map/MapPiece.hpp"

class BaseRoom : public MapPiece {
public:
    BaseRoom(RoomAssemblyConfig config);

    explicit BaseRoom(glm::vec2 cooridinate);
};

#endif
