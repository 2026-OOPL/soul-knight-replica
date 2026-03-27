#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>

#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

class Character : public IMapObject, public IStateful, public Util::GameObject {
public:
    Character(
        std::shared_ptr<Util::Animation> StandAnimation,
        std::shared_ptr<Util::Animation> WalkAnimation,
        std::shared_ptr<Util::Animation> DieAnimation,
        int zIndex
    );

    Character(
        const std::vector<std::string>& StandSprite,
        const std::vector<std::string>& WalkSprite,
        const std::vector<std::string>& DieSprite,
        int zIndex
    );

    // These are the getter of the m_Transform in Util::GameObject
    glm::vec2 GetAbsoluteScale() override;
    Util::Transform GetObjectTransform() override;

    // These are the getter of the m_AbsoluteTransform in MapSystem
    glm::vec2 GetAbsolutePosition() const;
    Util::Transform GetAbsoluteTransform() override;

    void SetLookDirectionByMoveIntent(glm::vec2 moveIntent);
    void SetSpriteTypeByMoveIntent(glm::vec2 moveIntent);


    // Absolute transform setter 
    void SetAbsoluteScale(glm::vec2 scale);
    void SetAbsoluteTransform(glm::vec2 transform);
    void SetAbsoluteRotation(float degree);

protected:
    float m_PlayerSpeed = 0.15F;
    
    Util::Transform m_AbsoluteTransform; // The absolute position on the map

    std::shared_ptr<Util::Animation> m_DieAnimation;
    std::shared_ptr<Util::Animation> m_StandAnimation;
    std::shared_ptr<Util::Animation> m_WalkAnimation;

};

#endif