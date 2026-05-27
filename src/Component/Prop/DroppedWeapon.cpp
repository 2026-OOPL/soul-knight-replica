#include <algorithm>
#include <limits>
#include <memory>
#include <utility>

#include <glm/geometric.hpp>

#include "Component/Prop/DroppedWeapon.hpp"

#include "Component/Map/MapSystem.hpp"
#include "Component/Player/Player.hpp"
#include "Component/Weapon.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

DroppedWeapon::DroppedWeapon(
    const glm::vec2 &coordinate,
    std::shared_ptr<Weapon> weapon,
    Config config
) : m_Weapon(std::move(weapon)),
    m_PickupRadius(std::max(0.0F, config.pickupRadius)) {
    this->SetAbsoluteTranslation(coordinate);
    this->SetZIndex(static_cast<float>(config.zIndex));

    if (this->m_Weapon != nullptr) {
        this->m_Weapon->SetSocketOffset({0.0F, 0.0F});
        this->m_Weapon->SetFacingDirection({1.0F, 0.0F});
        this->m_Weapon->SetAnchorPoint(coordinate);
        this->AddChild(this->m_Weapon);
    }
}

DroppedWeapon::DroppedWeapon(
    const glm::vec2 &coordinate,
    std::shared_ptr<Weapon> weapon
) : DroppedWeapon(coordinate, std::move(weapon), Config{}) {
}

void DroppedWeapon::Initialize(MapSystem *mapSystem) {
    this->m_MapSystem = mapSystem;
}

void DroppedWeapon::Update() {
    if (this->IsDestroyRequested() || this->m_MapSystem == nullptr ||
        this->m_Weapon == nullptr) {
        return;
    }

    this->m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
    this->m_Weapon->SetFacingDirection({1.0F, 0.0F});

    if (!Util::Input::IsKeyDown(Util::Keycode::E)) {
        return;
    }

    const std::shared_ptr<Player> player = this->FindPlayerInPickupRange();
    if (player == nullptr) {
        return;
    }

    const glm::vec2 dropPosition = player->GetAbsoluteTranslation();
    std::shared_ptr<Weapon> replacedWeapon =
        player->PickupWeapon(this->TakeWeapon());

    this->RequestDestroy();

    if (replacedWeapon != nullptr) {
        this->m_MapSystem->DropWeapon(std::move(replacedWeapon), dropPosition);
    }
}

std::shared_ptr<Player> DroppedWeapon::FindPlayerInPickupRange() const {
    if (this->m_MapSystem == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Player> nearestPlayer = nullptr;
    float bestDistanceSquared = std::numeric_limits<float>::max();
    const float pickupRangeSquared = this->m_PickupRadius * this->m_PickupRadius;

    for (const auto &player : this->m_MapSystem->GetPlayers()) {
        if (player == nullptr || player->IsDead()) {
            continue;
        }

        const glm::vec2 delta =
            player->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        const float distanceSquared = glm::dot(delta, delta);
        if (distanceSquared > pickupRangeSquared ||
            distanceSquared >= bestDistanceSquared) {
            continue;
        }

        bestDistanceSquared = distanceSquared;
        nearestPlayer = player;
    }

    return nearestPlayer;
}

std::shared_ptr<Weapon> DroppedWeapon::TakeWeapon() {
    std::shared_ptr<Weapon> weapon = std::move(this->m_Weapon);
    if (weapon != nullptr) {
        this->RemoveChild(weapon);
    }
    return weapon;
}
