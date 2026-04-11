#ifndef MAP_PIECE_HPP
#define MAP_PIECE_HPP

#include <memory>
#include <string>

#include <glm/vec2.hpp>

#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Transform.hpp"

#include "Common/MapObject.hpp"

class MapPiece : public Util::GameObject, public MapObject {
public:
    MapPiece(
        glm::vec2 cooridinate,
        const std::shared_ptr<Core::Drawable> &drawable
    );

    MapPiece(
        glm::vec2 cooridinate,
        std::string resource
    );

    glm::vec2 GetColliderSize() const { return this->m_ColliderSize; }

    void SetColliderSize(const glm::vec2 &colliderSize) {
        this->m_ColliderSize = colliderSize;
    }

    Util::Transform GetObjectTransform() const override;

private:
    std::shared_ptr<Util::Image> m_Image;
    glm::vec2 m_ColliderSize = {48.0F, 48.0F};
};

#endif
