#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <functional>
#include <memory>
#include <string>

#include "Common/CombatFaction.hpp"
#include "Common/MapObject.hpp"
#include "Component/IStateful.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

enum class WeaponType {
    RANGED,
    MELEE
};

class Bullet;
class Weapon : public Util::GameObject, public MapObject, public IStateful {
public:
    Weapon(
        std::string resource,
        int fireDelay
    );

    void Update() override;

    glm::vec2 GetFacingDirection();
    void SetFacingDirection(glm::vec2 direction);

    glm::vec2 GetAnchorPoint();
    void SetAnchorPoint(glm::vec2 anchor);
    
    Util::Transform GetObjectTransform() const override;

    bool ShotBullet();
    int GetAmmoCostPerShot() const;
    void SetAmmoCostPerShot(int ammoCostPerShot);
    int GetBulletDamage() const;
    void SetBulletDamage(int bulletDamage);
    CombatFaction GetProjectileFaction() const;
    void SetProjectileFaction(CombatFaction projectileFaction);

    void SetOnBulletFired(std::function<void(std::shared_ptr<Bullet>)> callback);

    virtual WeaponType GetWeaponType() = 0;
    
protected:
    void SetWeaponPointingByMoveDirection();
    void TriggerRecoil(float durationMs = 80.0F);
    float m_LastShotTime = 0;
    float m_WeaponRadius = 20;
    float m_RecoilDistance = 6;
    float m_RecoilEndTime = 0;
    
    glm::vec2 m_AnchorPoint = glm::vec2(0 ,0);
    glm::vec2 m_FacingDirection = glm::vec2(1,0);

    std::function<void(std::shared_ptr<Bullet>)> m_OnBulletFired;
    
    std::shared_ptr<Util::Image> m_Resource;
    
private:
    int m_FireDelay;
    int m_AmmoCostPerShot = 1;
    int m_BulletDamage = 100;
    CombatFaction m_ProjectileFaction = CombatFaction::Neutral;
};

#endif
