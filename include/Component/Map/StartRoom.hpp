#include "Component/Map/MapPiece.hpp"

class StarterRoom : public MapPiece {
    StarterRoom(glm::vec2 cooridinate, std::string resource) : MapPiece(cooridinate, resource) {
        this->m_Cooridinate = cooridinate;
        
        m_Image = std::make_shared<Util::Image>(resource);
        this->SetDrawable(m_Image);
    };
};