#include <glm/vec2.hpp>

#include "Component/Mobs/Mob.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Character/Character.hpp"
#include "Util/Logger.hpp"

void Mob::Update() {
    m_AI->Update();

    if (m_AI->GetAttackTrigger()) {
        bool result = m_Weapon->ShotBullet();
        if (result) LOG_INFO(result);
    }

    Character::Update();
}

glm::vec2 Mob::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}

glm::vec2 Mob::GetFaceDirection() const {
    return m_AI->GetFaceDirection();
}

void Mob::Initialize(MapSystem* mapSystem) {
    m_MapSystem = mapSystem;

    if (m_Weapon != nullptr) {
        m_Weapon->SetOnBulletFired(
            [mapSystem](std::shared_ptr<Bullet> bullet) {
                mapSystem->AddBullet(bullet);
            }
        );

    }
}