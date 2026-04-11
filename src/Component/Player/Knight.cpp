#include "Component/Player/Knight.hpp"

#include <algorithm>
#include <glm/geometric.hpp>
#include <memory>
#include <utility>

#include "Component/Player/Player.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"


Knight::Knight(
    std::function<std::weak_ptr<Character>()> GetNearestMob
) : Player(
    KnightPlayer::STAND_SPRITES,
    KnightPlayer::WALK_SPRITES,
    KnightPlayer::DIE_SPRITES
) {
    this->m_GetNearestMob = GetNearestMob;
}

glm::vec2 Knight::GetFaceDirection() const {
    std::shared_ptr<Character> mob = m_GetNearestMob().lock();

    glm::vec2 mobPosition = mob->GetAbsoluteTranslation();
    glm::vec2 playerPosition = this->GetAbsoluteTranslation();

    if (glm::distance(mobPosition, playerPosition) > 120) {
        return m_LastMomentum;
    }
    
    return mob->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
}