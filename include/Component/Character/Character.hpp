#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>

#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Component/Weapon.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"

class Character : public MapObject, public IStateful, public Util::GameObject {
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

    // Override for MapObject
    Util::Transform GetObjectTransform() const override; 

    std::shared_ptr<Weapon> GetWeapon();
    void SetWeapon(std::shared_ptr<Weapon> weapon);

    void Update() override;

    virtual glm::vec2 GetMoveIntent() const;

protected:
    float m_Health = 10;
    
    float m_PlayerSpeed = 0.15F;

    glm::vec2 m_LastMomentum;

    std::shared_ptr<Weapon> m_Weapon;
 
    std::shared_ptr<Util::Animation> m_DieAnimation;
    std::shared_ptr<Util::Animation> m_StandAnimation;
    std::shared_ptr<Util::Animation> m_WalkAnimation;

private:
    void SetLookDirectionByMoveIntent(glm::vec2 moveIntent);
    void SetSpriteTypeByMoveIntent(glm::vec2 moveIntent);
};

#endif