#ifndef PORTAL_HPP
#define PORTAL_HPP

#include <memory>
#include <glm/vec2.hpp>

#include "Component/Collision/ICollidable.hpp"
#include "Component/Prop/Prop.hpp"
#include "Util/Image.hpp"

class Portal : public Prop, public ICollidable {
public:
    Portal(
        glm::vec2 cooridinate
    );

    void Update() override;
    void Initialize(MapSystem *mapSystem) override;

    glm::vec2 GetCollisionOrigin() const override;
    void OnCollision(const Collision::CollisionSituation &situation) override;
    const std::vector<Collision::CollisionBox> &GetCollisionBoxes() const override;

    bool GetPlayerEntered() const;

private:
    std::shared_ptr<Util::Image> m_PortalImage;

    bool m_PlayerEntered = false;

    std::vector<Collision::CollisionBox> m_CollisionBoxes;
};

#endif 