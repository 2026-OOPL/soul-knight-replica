#include "Component/Mobs/GoblinGuard.hpp"
#include "Component/Character/Character.hpp"
#include <glm/ext/vector_float2.hpp>

void GoblinGuard::Update() {
    m_AI->Update();

    Character::Update();
}

glm::vec2 GoblinGuard::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}

glm::vec2 GoblinGuard::GetFaceDirection() const {
    return m_AI->GetFaceDirection();
}