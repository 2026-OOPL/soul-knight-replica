#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>

#include <glm/geometric.hpp>

#include "Component/Prop/AmmoOrb.hpp"

#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Core/Drawable.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

namespace {

constexpr char kAmmoOrbSprite[] = RESOURCE_DIR "/Drops/BulletBall.png";
constexpr float kMaxOrbDeltaTimeMs = 50.0F;

glm::vec2 SafeScaleForSize(
    const std::shared_ptr<Core::Drawable> &drawable,
    const glm::vec2 &renderSize
) {
    if (drawable == nullptr) {
        return {1.0F, 1.0F};
    }

    const glm::vec2 drawableSize = drawable->GetSize();
    return {
        renderSize.x / std::max(drawableSize.x, 1.0F),
        renderSize.y / std::max(drawableSize.y, 1.0F)
    };
}

} // namespace

AmmoOrb::AmmoOrb(const glm::vec2 &coordinate, Config config)
    : m_RenderSize(
          config.renderSize.x > 0.0F && config.renderSize.y > 0.0F ?
              config.renderSize :
              glm::vec2(18.0F, 18.0F)
      ),
      m_AmmoAmount(std::max(0, config.ammoAmount)),
      m_LingerRemainingMs(std::max(0.0F, config.lingerDurationMs)),
      m_AttractRadius(std::max(0.0F, config.attractRadius)),
      m_CollectRadius(std::max(0.0F, config.collectRadius)),
      m_Acceleration(std::max(0.0F, config.acceleration)),
      m_MaxSpeed(std::max(0.0F, config.maxSpeed)) {
    this->m_Image = std::make_shared<Util::Image>(kAmmoOrbSprite, false);
    this->SetDrawable(this->m_Image);
    this->SetAbsoluteScale(SafeScaleForSize(this->m_Image, this->m_RenderSize));
    this->SetAbsoluteTranslation(coordinate);
    this->SetZIndex(static_cast<float>(config.zIndex));
}

AmmoOrb::AmmoOrb(const glm::vec2 &coordinate)
    : AmmoOrb(coordinate, Config{}) {
}

void AmmoOrb::Initialize(MapSystem *mapSystem) {
    this->m_MapSystem = mapSystem;
}

void AmmoOrb::Update() {
    if (this->IsDestroyRequested() || this->m_MapSystem == nullptr) {
        return;
    }

    const float deltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), kMaxOrbDeltaTimeMs);
    if (deltaTimeMs <= 0.0F) {
        return;
    }

    if (this->m_LingerRemainingMs > 0.0F) {
        this->m_LingerRemainingMs =
            std::max(0.0F, this->m_LingerRemainingMs - deltaTimeMs);
        return;
    }

    const std::shared_ptr<Player> target = this->FindTargetPlayer();
    if (target == nullptr) {
        this->m_CurrentSpeed = 0.0F;
        return;
    }

    const glm::vec2 toPlayer =
        target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
    const float distanceSquared = glm::dot(toPlayer, toPlayer);
    if (distanceSquared <= this->m_CollectRadius * this->m_CollectRadius) {
        target->RecoverAmmo(this->m_AmmoAmount);
        this->RequestDestroy();
        return;
    }

    if (distanceSquared <= 0.0F) {
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const glm::vec2 direction = toPlayer / distance;
    this->m_CurrentSpeed = std::min(
        this->m_CurrentSpeed + this->m_Acceleration * deltaTimeMs,
        this->m_MaxSpeed
    );

    const glm::vec2 frameDelta = direction * this->m_CurrentSpeed * deltaTimeMs;
    if (glm::dot(frameDelta, frameDelta) >= distanceSquared) {
        target->RecoverAmmo(this->m_AmmoAmount);
        this->RequestDestroy();
        return;
    }

    this->SetAbsoluteTranslation(this->GetAbsoluteTranslation() + frameDelta);
}

bool AmmoOrb::CanTargetPlayer(const std::shared_ptr<Player> &player) const {
    if (player == nullptr || player->IsDead()) {
        return false;
    }

    const glm::vec2 playerPos = player->GetAbsoluteTranslation();
    const glm::vec2 delta = playerPos - this->GetAbsoluteTranslation();
    return glm::dot(delta, delta) <= this->m_AttractRadius * this->m_AttractRadius;
}

std::shared_ptr<Player> AmmoOrb::FindTargetPlayer() const {
    if (this->m_MapSystem == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Player> target = nullptr;
    float bestDistanceSquared = std::numeric_limits<float>::max();

    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (!this->CanTargetPlayer(player)) {
            continue;
        }

        const glm::vec2 delta =
            player->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        const float distanceSquared = glm::dot(delta, delta);
        if (distanceSquared >= bestDistanceSquared) {
            continue;
        }

        bestDistanceSquared = distanceSquared;
        target = player;
    }

    return target;
}
