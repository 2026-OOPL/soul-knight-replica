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

class MapPiece : public Util::GameObject, public IMapObject {
public:
    MapPiece(
        glm::vec2 cooridinate,
        const std::shared_ptr<Core::Drawable> &drawable,
        bool isWall = false
    );

    MapPiece(glm::vec2 cooridinate, std::string resource, bool isWall = false);

    glm::vec2 GetObjectSize() override;
    glm::vec2 GetCooridinate() override;
    Util::Transform GetTransform() override;

    void SetTransformByCooridinate(glm::vec2 cooridinate);//目前此職責已交給carmera

    bool IsWall() const { return this->m_IsWall; }
    void SetIsWall(bool isWall) { this->m_IsWall = isWall; }

    glm::vec2 GetPosition() const { return this->m_Cooridinate; }

    glm::vec2 GetColliderSize() const { return this->m_ColliderSize; }
    void SetColliderSize(const glm::vec2 &colliderSize) {
        this->m_ColliderSize = colliderSize;
    }

private:
    glm::vec2 m_Cooridinate;
    std::shared_ptr<Util::Image> m_Image;
    bool m_IsWall = false;
    glm::vec2 m_ColliderSize = {0.0F, 0.0F};
};

#endif
