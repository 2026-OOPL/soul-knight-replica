#include "Component/Mobs/GoblinGuard.hpp"
#include "Component/Character/Character.hpp"
#include <glm/ext/vector_float2.hpp>

void GoblinGuard::Update() {
    m_AI->Update();

    this->m_FacingDirection = m_AI->GetFaceDirection();

    Character::Update();
}

glm::vec2 GoblinGuard::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}