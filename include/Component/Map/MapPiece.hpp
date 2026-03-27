#ifndef MAP_PIECE_HPP
#define MAP_PIECE_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <sys/types.h>

#include "Component/IMapObject.hpp"
#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Transform.hpp"

class MapPiece : public Util::GameObject, public IMapObject {
public:
    MapPiece(
        glm::vec2 cooridinate,
        const std::shared_ptr<Core::Drawable> &drawable
    );

    MapPiece(
        glm::vec2 cooridinate,
        std::string resource
    );
    
    glm::vec2 GetAbsoluteScale() override;
    glm::vec2 GetAbsoluteCooridinate() const { return this->m_AbsoluteTransform.translation; }
    Util::Transform GetAbsoluteTransform() override;
    
    Util::Transform GetObjectTransform() override;
    
    glm::vec2 GetColliderSize() const { return this->m_ColliderSize; }
    void SetColliderSize(const glm::vec2 &colliderSize) {
        this->m_ColliderSize = colliderSize;
    }
    
    Util::Transform m_AbsoluteTransform;

private:

    std::shared_ptr<Util::Image> m_Image;//圖片路徑
    glm::vec2 m_ColliderSize = {48.0F, 48.0F};//碰撞盒大小

};

#endif
