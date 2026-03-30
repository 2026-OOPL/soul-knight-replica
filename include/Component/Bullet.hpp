#ifndef BULLET_HPP
#define BULLET_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>

#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon/Weapon.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

class Bullet : public MapObject, public IStateful, public Util::GameObject {
public:
    Bullet(
        std::shared_ptr<Util::Animation> animation,
        glm::vec2 cooridinate,
        glm::vec2 momentum,
        int zIndex
    );

    Bullet(
        const std::vector<std::string>& sprite,
        glm::vec2 cooridinate,
        glm::vec2 momentum,
        int zIndex
    );

    // Override for MapObject
    Util::Transform GetObjectTransform() const override; 

    void Update() override;

protected:
    glm::vec2 m_Momentum;

    std::shared_ptr<Util::Animation> m_Animation;
};

#endif