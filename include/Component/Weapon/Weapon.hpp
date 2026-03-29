#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <memory>
#include <string>

#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

class Bullet;
class Weapon : public Util::GameObject, public MapObject, public IStateful {
public:
    Weapon(
        std::string resource
    );

    void Update() override;

    glm::vec2 GetFacingDirection();
    void SetFacingDirection(glm::vec2 direction);

    glm::vec2 GetAnchorPoint();
    void SetAnchorPoint(glm::vec2 anchor);
    
    Util::Transform GetObjectTransform() const override;

    void ShotBullet();

    void SetOnBulletFired(std::function<void(std::shared_ptr<Bullet>)> callback);

protected:
    void SetWeaponPointingByMoveDirection();
    float m_WeaponRadius = 20;
    
    glm::vec2 m_AnchorPoint = glm::vec2(0 ,0);
    glm::vec2 m_FacingDirection = glm::vec2(1,0);

    std::function<void(std::shared_ptr<Bullet>)> m_OnBulletFired;
    
    std::shared_ptr<Util::Image> m_Resource;
    float m_LastShotTime = 0;
};

#endif