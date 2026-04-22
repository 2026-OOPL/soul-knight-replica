#include <algorithm>
#include <memory>
#include <utility>

#include <glm/geometric.hpp>

#include "Component/Prop/Chest.hpp"

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Core/Drawable.hpp"

namespace {

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

Chest::Chest(const glm::vec2 &coordinate, Config config)
    : m_Visuals(std::move(config.visuals)),
      m_Reward(std::move(config.reward)),
      m_RenderSize(
          config.renderSize.x > 0.0F && config.renderSize.y > 0.0F ?
              config.renderSize :
              config.blockingSize
      ),
      m_AutoOpenRange(std::max(0.0F, config.autoOpenRange)) {
    this->SetAbsoluteTranslation(coordinate);
    this->SetOwningRoom(config.owningRoom);
    this->SetBlockingSize(config.blockingSize);
    this->SetBlockingOffset(config.blockingOffset);
    this->SetZIndex(static_cast<float>(config.zIndex));
    this->ApplyCurrentVisual();
}

void Chest::Initialize(MapSystem *mapSystem) {
    this->m_MapSystem = mapSystem;
}

void Chest::Update() {
    if (this->m_IsOpen || this->m_MapSystem == nullptr) {
        return;
    }

    const std::shared_ptr<BaseRoom> owningRoom = this->GetOwningRoom();
    const float openRangeSquared = this->m_AutoOpenRange * this->m_AutoOpenRange;

    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (player == nullptr) {
            continue;
        }

        const glm::vec2 playerPos = player->GetAbsoluteTranslation();
        if (owningRoom != nullptr && !owningRoom->IsPlayerInside(playerPos)) {
            continue;
        }

        const glm::vec2 delta = playerPos - this->GetAbsoluteTranslation();
        if (glm::dot(delta, delta) > openRangeSquared) {
            continue;
        }

        this->Open(*player);
        return;
    }
}

bool Chest::IsOpen() const {
    return this->m_IsOpen;
}

void Chest::SetReward(const std::shared_ptr<IChestReward> &reward) {
    this->m_Reward = reward;
}

void Chest::Open(Player &player) {
    if (this->m_IsOpen) {
        return;
    }

    this->m_IsOpen = true;
    this->ApplyCurrentVisual();

    if (this->m_Reward != nullptr && this->m_MapSystem != nullptr) {
        this->m_Reward->Grant(player, *this->m_MapSystem);
        this->m_Reward.reset();
    }
}

void Chest::ApplyCurrentVisual() {
    if (this->m_IsOpen) {
        this->ApplyDrawable(
            this->m_Visuals.openIdle != nullptr ?
                this->m_Visuals.openIdle :
                this->m_Visuals.closedIdle
        );
        return;
    }

    this->ApplyDrawable(this->m_Visuals.closedIdle);
}

void Chest::ApplyDrawable(const std::shared_ptr<Core::Drawable> &drawable) {
    this->SetDrawable(drawable);
    this->SetAbsoluteScale(SafeScaleForSize(drawable, this->m_RenderSize));
}
