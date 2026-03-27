#ifndef ROOM_17x23_HPP
#define ROOM_17x23_HPP

#include <glm/vec2.hpp>

#include "Component/Map/MapPiece.hpp"

class Room17x13 : public MapPiece {
    Room17x13(glm::vec2 cooridinate) : MapPiece(
        cooridinate,
        RESOURCE_DIR "/Map/Room/Room_17x23.png"
    ) {
        
    };

};

#endif