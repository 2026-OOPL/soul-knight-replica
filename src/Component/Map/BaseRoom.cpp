#include "Component/Map/BaseRoom.hpp"
#include "Common/Enums.hpp"
#include "Util/Image.hpp"
#include <memory>

BaseRoom::BaseRoom(glm::vec2 cooridinate)
    : MapPiece(cooridinate, RESOURCE_DIR "/Map/Room/Room_17x23.png") {
}

BaseRoom::BaseRoom(RoomAssemblyConfig config) 
: MapPiece(config.roomCenter, RESOURCE_DIR "/Map/Room/Room_17x23.png") {
    switch (config.type) {
        case RoomType::ROOM_13_13:
            this->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Map/Room/Room_13x13.png"));
            break;

        case RoomType::ROOM_15_15:
            this->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Map/Room/Room_15x15.png"));
            break;

        case RoomType::ROOM_17_17: 
            this->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Map/Room/Room_17x17.png"));
            break;

        case RoomType::ROOM_17_23:
            this->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Map/Room/Room_17x23.png"));
            break;

        case RoomType::ROOM_23_17:
            this->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Map/Room/Room_23x17.png"));
            break;

    }
    
}