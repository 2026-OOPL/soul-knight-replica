#include "Component/Map/BaseRoom.hpp"

BaseRoom::BaseRoom(glm::vec2 cooridinate)
    : MapPiece(cooridinate, RESOURCE_DIR "/Map/Room/Room_15x15.png") {
}
