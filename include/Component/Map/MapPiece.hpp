#ifndef MAP_PIECE_HPP
#define MAP_PIECE_HPP

#include "Core/Context.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <string>

class MapPiece : public Util::GameObject {
public:
    MapPiece(glm::vec2 cooridinate, std::string resource) {
        this->m_Cooridinate = cooridinate;
        
        m_Image = std::make_shared<Util::Image>(resource);
        this->SetDrawable(m_Image);
    };

    glm::vec2 m_Cooridinate;
    std::shared_ptr<Util::Image> m_Image;
    
    glm::vec2 GetScaledCooridinate();
    bool GetVisibilityByCooridinate(glm::vec2 cooridinate);
    void SetTransformByCooridinate(glm::vec2 cooridinate);
};

#endif