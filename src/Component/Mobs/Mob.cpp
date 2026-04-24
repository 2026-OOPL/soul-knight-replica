#include <glm/vec2.hpp>

#include "Util/Logger.hpp"

#include "Component/Mobs/Mob.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Character/Character.hpp"
#include "Component/AI/RangedAI.hpp"

Mob::Mob(
    const std::vector<std::string>& StandAnimation,
    const std::vector<std::string>& WalkAnimation,
    const std::vector<std::string>& DieAnimation,
    std::shared_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Character(
    StandAnimation,
    WalkAnimation,
    DieAnimation,
    4,
    CombatFaction::Enemy
) {
    m_TracePlayerTemp = tracePlayer;
    m_CollisionSystemTemp = collisionSystem;

    this->m_AI = std::make_shared<RangedAI>(this, m_TracePlayerTemp, m_CollisionSystemTemp);

    this->m_PlayerSpeed = 0.05F;

}

void Mob::Update() {
    m_AI->Update();
    Character::Update();

    if (m_AI->GetAttackDirection() != glm::vec2(0.0F)) {
        bool result = m_Weapon->ShotBullet();
        if (result) {
            this->TriggerAttackVisual();
        }
    }
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
