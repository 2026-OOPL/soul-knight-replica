#include "Generator/MapGenerator.hpp"

Direction MapGenerator::GetDirection() {
    double seed = m_Dist(m_Engine);
    if (seed < 0.25) {
        return Direction::BOTTOM;
    }

    if (seed < 0.5) {
        return Direction::LEFT;
    }

    if (seed < 0.75) {
        return Direction::RIGHT;
    }

    return Direction::TOP;
}

void MapGenerator::GenerateRoom() {
    Direction starterRoomDirection = GetDirection();

    switch (starterRoomDirection) {
        case Direction::BOTTOM:
            
    }

    

    rooms.push_back("STARTER_ROOM");
}