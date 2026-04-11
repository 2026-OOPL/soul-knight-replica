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
    KnightPlayer::DIE_SPRITES,
    KnightPlayer::MAX_HEALTH,
    KnightPlayer::MAX_SHIELD,
    KnightPlayer::MAX_AMMO
) {
    this->m_GetNearestMob = std::move(GetNearestMob);
}

glm::vec2 Knight::GetFaceDirection() const {
    std::shared_ptr<Character> mob = m_GetNearestMob().lock();
    if (mob == nullptr) {
        return m_LastMomentum;
    }

    const glm::vec2 mobPosition = mob->GetAbsoluteTranslation();
    const glm::vec2 playerPosition = this->GetAbsoluteTranslation();

    if (glm::distance(mobPosition, playerPosition) > 120.0F) {
        return m_LastMomentum;
    }

    return mobPosition - playerPosition;
}
