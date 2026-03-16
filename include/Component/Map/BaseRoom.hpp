#include "Component/Map/MapPiece.hpp"

class BaseRoom : public MapPiece {
public:
    BaseRoom(glm::vec2 cooridinate) 
    : MapPiece(cooridinate, RESOURCE_DIR"/Map/Room/Room_17x17.png") {
        
    };


};