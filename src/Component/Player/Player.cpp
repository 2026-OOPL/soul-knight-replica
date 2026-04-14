#include <algorithm>
#include <utility>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

#include "Component/Player/Player.hpp"

Player::Player(
    const std::vector<std::string>& StandSprite,
    const std::vector<std::string>& WalkSprite,
    const std::vector<std::string>& DieSprite,
    int maxHealth,
    int maxShield,
    int maxAmmo
) : Character(
    StandSprite,
    WalkSprite,
    DieSprite,
    4
) {
    this->SetMaxHealth(maxHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetMaxShield(maxShield);
    this->SetCurrentShield(this->GetMaxShield());
    this->SetMaxAmmo(maxAmmo);
    this->SetCurrentAmmo(this->GetMaxAmmo());
    this->SetFaction(CombatFaction::Player);
    this->BindWeaponAmmoConsumer();

    this->m_AbsoluteTransform.translation = {0.0F, 0.0F};

    Collision::CollisionFilter filter = this->GetCollisionFilter();
    filter.layer = Collision::CollisionLayer::Player;
    filter.mask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop) |
        Collision::ToMask(Collision::CollisionLayer::Enemy) |
        Collision::ToMask(Collision::CollisionLayer::EnemyProjectile) |
        Collision::ToMask(Collision::CollisionLayer::Trigger);
    filter.blocking = true;
    this->SetCollisionFilter(filter);
}

glm::vec2 Player::GetMoveIntent() const {
    glm::vec2 moveIntent(0.0F, 0.0F);

    if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
        moveIntent.y += 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) {
        moveIntent.y -= 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        moveIntent.x -= 1.0F;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        moveIntent.x += 1.0F;
    }

    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        return moveIntent;
    }

    return glm::normalize(moveIntent);
}

void Player::Update() {
    Character::Update();

    if (this->IsDead() || this->GetMaxShield() <= 0) {
        this->m_ShieldRegenDelayRemainingMs = 0.0F;
        this->m_ShieldRegenElapsedMs = 0.0F;
        return;
    }

    if (this->GetCurrentShield() >= this->GetMaxShield()) {
        this->m_ShieldRegenDelayRemainingMs = 0.0F;
        this->m_ShieldRegenElapsedMs = 0.0F;
        return;
    }

    float deltaTimeMs = Util::Time::GetDeltaTimeMs();

    if (this->m_ShieldRegenDelayRemainingMs > 0.0F) {
        if (deltaTimeMs < this->m_ShieldRegenDelayRemainingMs) {
            this->m_ShieldRegenDelayRemainingMs -= deltaTimeMs;
            return;
        }

        deltaTimeMs -= this->m_ShieldRegenDelayRemainingMs;
        this->m_ShieldRegenDelayRemainingMs = 0.0F;
    }

    this->m_ShieldRegenElapsedMs += deltaTimeMs;

    while (this->m_ShieldRegenElapsedMs >= kShieldRegenIntervalMs) {
        this->RestoreShield(1);
        this->m_ShieldRegenElapsedMs -= kShieldRegenIntervalMs;

        if (this->GetCurrentShield() >= this->GetMaxShield()) {
            this->m_ShieldRegenDelayRemainingMs = 0.0F;
            this->m_ShieldRegenElapsedMs = 0.0F;
            break;
        }
    }
}

void Player::SetWeapon(std::shared_ptr<Weapon> weapon) {
    if (this->m_Weapon != nullptr) {
        this->m_Weapon->SetAmmoConsumer(nullptr);
    }

    Character::SetWeapon(std::move(weapon));
    this->BindWeaponAmmoConsumer();
}

void Player::ApplyDamage(int damage) {
    if (damage <= 0) {
        return;
    }

    this->m_ShieldRegenDelayRemainingMs = kShieldRegenDelayMs;
    this->m_ShieldRegenElapsedMs = 0.0F;

    const int shieldDamage = std::min(this->m_CurrentShield, damage);
    this->SetCurrentShield(this->m_CurrentShield - shieldDamage);
    Character::ApplyDamage(damage - shieldDamage);
}

int Player::GetCurrentShield() const {
    return this->m_CurrentShield;
}

int Player::GetMaxShield() const {
    return this->m_MaxShield;
}

void Player::SetCurrentShield(int shield) {
    this->m_CurrentShield = std::clamp(shield, 0, this->m_MaxShield);
}

void Player::SetMaxShield(int maxShield) {
    this->m_MaxShield = std::max(0, maxShield);
    this->m_CurrentShield = std::clamp(this->m_CurrentShield, 0, this->m_MaxShield);
}

void Player::RestoreShield(int amount) {
    if (amount <= 0) {
        return;
    }

    this->SetCurrentShield(this->m_CurrentShield + amount);
}

int Player::GetCurrentAmmo() const {
    return this->m_CurrentAmmo;
}

int Player::GetMaxAmmo() const {
    return this->m_MaxAmmo;
}

void Player::SetCurrentAmmo(int ammo) {
    this->m_CurrentAmmo = std::clamp(ammo, 0, this->m_MaxAmmo);
}

void Player::SetMaxAmmo(int maxAmmo) {
    this->m_MaxAmmo = std::max(0, maxAmmo);
    this->m_CurrentAmmo = std::clamp(this->m_CurrentAmmo, 0, this->m_MaxAmmo);
}

bool Player::TryConsumeAmmo(int amount) {
    if (amount <= 0) {
        return true;
    }

    if (this->m_CurrentAmmo < amount) {
        return false;
    }

    this->m_CurrentAmmo -= amount;
    return true;
}

void Player::RecoverAmmo(int amount) {
    if (amount <= 0) {
        return;
    }

    this->SetCurrentAmmo(this->m_CurrentAmmo + amount);
}

PlayerHudState Player::GetHudState() const {
    return {
        this->GetCurrentHealth(),
        this->GetMaxHealth(),
        this->GetCurrentShield(),
        this->GetMaxShield(),
        this->GetCurrentAmmo(),
        this->GetMaxAmmo()
    };
}

void Player::BindWeaponAmmoConsumer() {
    if (this->m_Weapon == nullptr) {
        return;
    }

    this->m_Weapon->SetAmmoConsumer(
        [this](int amount) {
            return this->TryConsumeAmmo(amount);
        }
    );
}
