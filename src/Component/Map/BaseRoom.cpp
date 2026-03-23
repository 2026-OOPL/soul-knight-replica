#include "Component/Map/BaseRoom.hpp"

BaseRoom::BaseRoom(glm::vec2 cooridinate)
    : MapPiece(cooridinate, RESOURCE_DIR "/Map/Room/Room_17x17.png") {
}
