#ifndef JOINT_ROOM_GEN
#define JOINT_ROOM_GEN

#include "MapGenerator.hpp"

class JointRoomGen : public MapGenerator {
    JointRoomGen(std::string seed) : MapGenerator(seed) {};
};

#endif