#include <algorithm>
#include <memory>
#include <utility>
#include <cstdlib>
#include <glm/vec2.hpp>

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

#include "Component/Weapon.hpp"
#include "Component/Bullet.hpp"
#include "Component/Bullets/TestBullet.hpp"

Weapon::Weapon(
    std::string resource,
    int fireDelay
) : Util::GameObject(nullptr, 5) {
    m_Resource = std::make_shared<Util::Image>(resource);

    this->m_FireDelay = fireDelay;

    this->SetDrawable(m_Resource);
}

void Weapon::SetFacingDirection(glm::vec2 direction) {
    if (glm::length(direction) <= 0.0001F) {
        return;
    }

    this->m_FacingDirection = glm::normalize(direction);
    this->SetWeaponPointingByMoveDirection();
}

glm::vec2 Weapon::GetFacingDirection() {
    return this->m_FacingDirection;
}

void Weapon::Update() {
    // Apply the rotation for the weapon
    this->SetWeaponPointingByMoveDirection();
}

Util::Transform Weapon::GetObjectTransform() const {
    return this->m_Transform;
}

void Weapon::SetWeaponPointingByMoveDirection() {
    float rotation = atan2(m_FacingDirection.y, m_FacingDirection.x);
    const float recoilOffset =
        Util::Time::GetElapsedTimeMs() < this->m_RecoilEndTime ?
        -this->m_RecoilDistance :
        0.0F;

    // Set scale
    glm::vec2 currentScale = this->GetAbsoluteScale();
    float scaleX = std::abs(currentScale.x);
    float scaleY = std::abs(currentScale.y);

    if (fabs(rotation) > M_PI / 2.0f) {
        this->SetAbsoluteScale(glm::vec2(scaleX, -scaleY));
    } else {
        this->SetAbsoluteScale(glm::vec2(scaleX, scaleY));
    }

    this->SetAbsoluteRotation(rotation);

    // Set translation
    this->SetAbsoluteTranslation(
        m_AnchorPoint + m_FacingDirection * (m_WeaponRadius + recoilOffset)
    );
}

glm::vec2 Weapon::GetAnchorPoint() {
    return m_AnchorPoint;
}

void Weapon::SetAnchorPoint(glm::vec2 anchor) {
    this->m_AnchorPoint = anchor;
    this->SetWeaponPointingByMoveDirection();
}

int Weapon::GetAmmoCostPerShot() const {
    return this->m_AmmoCostPerShot;
}

void Weapon::SetAmmoCostPerShot(int ammoCostPerShot) {
    this->m_AmmoCostPerShot = std::max(0, ammoCostPerShot);
}

int Weapon::GetBulletDamage() const {
    return this->m_BulletDamage;
}

void Weapon::SetBulletDamage(int bulletDamage) {
    this->m_BulletDamage = std::max(0, bulletDamage);
}

CombatFaction Weapon::GetProjectileFaction() const {
    return this->m_ProjectileFaction;
}

void Weapon::SetProjectileFaction(CombatFaction projectileFaction) {
    this->m_ProjectileFaction = projectileFaction;
}

void Weapon::SetOnBulletFired(std::function<void(std::shared_ptr<Bullet>)> callback) {
    this->m_OnBulletFired = callback;
}

void Weapon::TriggerRecoil(float durationMs) {
    this->m_RecoilEndTime = Util::Time::GetElapsedTimeMs() + durationMs;
    this->SetWeaponPointingByMoveDirection();
}

std::shared_ptr<Bullet> Weapon::CreateBullet() const {
    std::shared_ptr<Bullet> bullet = std::make_shared<TestBullet>(
        this->GetAbsoluteTranslation(),
        this->m_FacingDirection,
        this->m_BulletDamage,
        this->m_ProjectileFaction
    );
    this->ConfigureBullet(bullet);
    return bullet;
}

void Weapon::ConfigureBullet(const std::shared_ptr<Bullet> &bullet) const {
    (void)bullet;
}

bool Weapon::ShotBullet() {
    this->SetWeaponPointingByMoveDirection();

    if (Util::Time::GetElapsedTimeMs() - m_LastShotTime < m_FireDelay) {
        return false;
    }

    m_LastShotTime = Util::Time::GetElapsedTimeMs();
    this->TriggerRecoil();

    std::shared_ptr<Bullet> bullet = this->CreateBullet();
    if (bullet == nullptr) {
        return false;
    }

    // Pass this bullet to map system to manage
    if (this->m_OnBulletFired != nullptr) {
        this->m_OnBulletFired(bullet);
    }
    
    return true;
}
