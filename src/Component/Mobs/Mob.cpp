#include "Component/Mobs/Mob.hpp"
#include "Component/Character/Character.hpp"
#include <glm/vec2.hpp>

void Mob::Update() {
    m_AI->Update();

    Character::Update();
}

glm::vec2 Mob::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}

glm::vec2 Mob::GetFaceDirection() const {
    return m_AI->GetFaceDirection();
}