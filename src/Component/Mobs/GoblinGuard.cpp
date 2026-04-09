#include "Component/Mobs/GoblinGuard.hpp"
#include "Component/Character/Character.hpp"

void GoblinGuard::Update() {

    m_AI->Update();

    Character::Update();
}

glm::vec2 GoblinGuard::GetMoveIntent() const {
    return m_AI->GetMoveDirection();
}