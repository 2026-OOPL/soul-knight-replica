#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "Component/Weapon.hpp"
#include "Common/Constants.hpp"
#include "Common/MapObject.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

#include "Component/Player/Player.hpp"

namespace {

const std::vector<std::string> kMeleeAttackEffectSprites = {
    RESOURCE_DIR"/Effect/meleeattacking/MeleeAttack_0.png",
    RESOURCE_DIR"/Effect/meleeattacking/MeleeAttack_1.png",
    RESOURCE_DIR"/Effect/meleeattacking/MeleeAttack_2.png",
    RESOURCE_DIR"/Effect/meleeattacking/MeleeAttack_3.png"
};

class PlayerMeleeAttackVisual : public Util::GameObject, public MapObject {
public:
    PlayerMeleeAttackVisual() : Util::GameObject(nullptr, 6) {
    }

    Util::Transform GetObjectTransform() const override {
        return this->m_Transform;
    }
};

glm::vec2 NormalizeOrRight(const glm::vec2 &direction) {
    if (glm::length(direction) <= 0.0001F) {
        return {1.0F, 0.0F};
    }

    return glm::normalize(direction);
}

} // namespace

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
    4,
    CombatFaction::Player
) {
    this->SetMaxHealth(maxHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetMaxShield(maxShield);
    this->SetCurrentShield(this->GetMaxShield());
    this->SetMaxAmmo(maxAmmo);
    this->SetCurrentAmmo(this->GetMaxAmmo());
    this->SetFaction(CombatFaction::Player);

    this->m_MeleeAttackAnimation = std::make_shared<Util::Animation>(
        kMeleeAttackEffectSprites,
        false,
        35,
        false,
        0,
        false
    );
    this->m_MeleeAttackVisual = std::make_shared<PlayerMeleeAttackVisual>();
    this->m_MeleeAttackVisual->SetVisible(false);
    this->AddChild(this->m_MeleeAttackVisual);

    this->m_AbsoluteTransform.translation = {0.0F, 0.0F};
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

float Player::GetMoveSpeedMultiplier() const {
    if (this->m_Weapon == nullptr ||
        !Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
        return 1.0F;
    }

    return this->m_Weapon->GetAttackMoveSpeedMultiplier();
}

void Player::UpdateWeaponPresentation() {
    if (this->m_Weapon == nullptr) {
        return;
    }

    this->m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
    this->m_Weapon->SetSocketOffset(this->m_WeaponSocketOffset);
    this->m_Weapon->SetFacingDirection(this->GetFaceDirection());
}

void Player::Update() {
    Character::Update();
    this->UpdateMeleeAttackVisual();

    if (Util::Input::IsKeyDown(Util::Keycode::T)) {
        this->m_HealthLocked = !this->m_HealthLocked;
        LOG_INFO(
            "Player health lock {}.",
            this->m_HealthLocked ? "enabled" : "disabled"
        );
    }

    if (Util::Input::IsKeyDown(Util::Keycode::Q)) {
        this->SwitchWeapon();
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
        if (!this->TryMeleeAttack() && this->m_Weapon != nullptr) {
            int cost = this->m_Weapon->GetAmmoCostPerShot();
            
            // Check if player has enough ammo before attempting to fire
            if (this->GetCurrentAmmo() >= cost || cost <= 0) {
                if (this->m_Weapon->ShotBullet()) {
                    this->TriggerAttackVisual();
                    this->TryConsumeAmmo(cost);
                }
            }
        }
    }
    
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
    (void)this->PickupWeapon(std::move(weapon));
}

std::shared_ptr<Weapon> Player::PickupWeapon(std::shared_ptr<Weapon> weapon) {
    if (weapon == nullptr) {
        this->m_WeaponSlots[this->m_ActiveWeaponSlot] = nullptr;
        Character::SetWeapon(nullptr);
        return nullptr;
    }

    this->ApplyWeaponBulletCallback(weapon);

    const int inactiveSlot = 1 - this->m_ActiveWeaponSlot;
    if (this->m_WeaponSlots[this->m_ActiveWeaponSlot] == nullptr) {
        this->m_WeaponSlots[this->m_ActiveWeaponSlot] = std::move(weapon);
        Character::SetWeapon(this->m_WeaponSlots[this->m_ActiveWeaponSlot]);
        return nullptr;
    }

    if (this->m_WeaponSlots[inactiveSlot] == nullptr) {
        this->m_WeaponSlots[inactiveSlot] = std::move(weapon);
        this->EquipWeaponSlot(inactiveSlot);
        return nullptr;
    }

    std::shared_ptr<Weapon> droppedWeapon =
        this->m_WeaponSlots[this->m_ActiveWeaponSlot];
    this->m_WeaponSlots[this->m_ActiveWeaponSlot] = std::move(weapon);
    Character::SetWeapon(this->m_WeaponSlots[this->m_ActiveWeaponSlot]);
    return droppedWeapon;
}

void Player::SetOnWeaponBulletFired(
    std::function<void(std::shared_ptr<Bullet>)> callback
) {
    this->m_OnWeaponBulletFired = std::move(callback);

    for (const auto &weapon : this->m_WeaponSlots) {
        this->ApplyWeaponBulletCallback(weapon);
    }
}

void Player::SetMeleeAttackResolver(MeleeAttackResolver resolver) {
    this->m_MeleeAttackResolver = std::move(resolver);
}

void Player::SwitchWeapon() {
    const int nextSlot = 1 - this->m_ActiveWeaponSlot;
    if (this->m_WeaponSlots[nextSlot] == nullptr) {
        return;
    }

    this->EquipWeaponSlot(nextSlot);
}

void Player::EquipWeaponSlot(int slotIndex) {
    if (slotIndex < 0 ||
        slotIndex >= static_cast<int>(this->m_WeaponSlots.size()) ||
        this->m_WeaponSlots[slotIndex] == nullptr) {
        return;
    }

    this->m_ActiveWeaponSlot = slotIndex;
    this->ApplyWeaponBulletCallback(this->m_WeaponSlots[this->m_ActiveWeaponSlot]);
    Character::SetWeapon(this->m_WeaponSlots[this->m_ActiveWeaponSlot]);
}

void Player::ApplyWeaponBulletCallback(const std::shared_ptr<Weapon> &weapon) {
    if (weapon == nullptr || this->m_OnWeaponBulletFired == nullptr) {
        return;
    }

    weapon->SetOnBulletFired(this->m_OnWeaponBulletFired);
}

bool Player::TryMeleeAttack() {
    if (this->IsMeleeAttacking()) {
        return true;
    }

    if (this->m_MeleeAttackResolver == nullptr ||
        Util::Time::GetElapsedTimeMs() - this->m_LastMeleeAttackTime <
            kMeleeAttackCooldownMs) {
        return false;
    }

    if (!this->m_MeleeAttackResolver(*this)) {
        return false;
    }

    this->m_LastMeleeAttackTime = Util::Time::GetElapsedTimeMs();
    this->TriggerAttackVisual(kMeleeAttackVisualDurationMs);
    this->StartMeleeAttackVisual();
    return true;
}

bool Player::IsMeleeAttacking() const {
    return Util::Time::GetElapsedTimeMs() < this->m_MeleeAttackVisualEndTime;
}

bool Player::GetMeleeAttackDebugBox(Collision::AxisAlignedBox &box) const {
    if (!this->IsMeleeAttacking()) {
        return false;
    }

    const glm::vec2 facingDirection = NormalizeOrRight(this->GetFaceDirection());
    const float attackRadius = MAP_PIXEL_PER_BLOCK * 2.4F;
    box.center = this->GetAbsoluteTranslation() + facingDirection * (attackRadius * 0.5F);
    box.size = {attackRadius, attackRadius};
    return true;
}

void Player::StartMeleeAttackVisual() {
    if (this->m_MeleeAttackVisual == nullptr ||
        this->m_MeleeAttackAnimation == nullptr) {
        return;
    }

    this->m_MeleeAttackVisualEndTime =
        Util::Time::GetElapsedTimeMs() + kMeleeAttackVisualDurationMs;
    this->m_MeleeAttackAnimation->SetCurrentFrame(0);
    this->m_MeleeAttackAnimation->Play();
    this->m_MeleeAttackVisual->SetDrawable(this->m_MeleeAttackAnimation);
    this->m_MeleeAttackVisual->SetVisible(true);
    this->UpdateMeleeAttackVisual();
}

void Player::UpdateMeleeAttackVisual() {
    if (this->m_MeleeAttackVisual == nullptr) {
        return;
    }

    if (!this->IsMeleeAttacking()) {
        this->m_MeleeAttackVisual->SetVisible(false);
        this->m_MeleeAttackVisual->SetDrawable(nullptr);
        return;
    }

    const glm::vec2 facingDirection = NormalizeOrRight(this->GetFaceDirection());
    const float rotation = std::atan2(facingDirection.y, facingDirection.x);
    const float attackRadius = MAP_PIXEL_PER_BLOCK * 2.4F;

    const std::shared_ptr<MapObject> mapVisual =
        std::dynamic_pointer_cast<MapObject>(this->m_MeleeAttackVisual);
    if (mapVisual == nullptr) {
        return;
    }

    mapVisual->SetAbsoluteTranslation(
        this->GetAbsoluteTranslation() + facingDirection * (attackRadius * 0.5F)
    );
    mapVisual->SetAbsoluteRotation(rotation);
    mapVisual->SetAbsoluteScale(
        {1.5F, std::abs(rotation) > 3.14159265358979323846F / 2.0F ? -1.5F : 1.5F}
    );
}

void Player::ApplyDamage(int damage) {
    if (damage <= 0) {
        return;
    }

    if (this->m_HealthLocked) {
        return;
    }

    this->m_ShieldRegenDelayRemainingMs = kShieldRegenDelayMs;
    this->m_ShieldRegenElapsedMs = 0.0F;

    const int shieldDamage = std::min(this->m_CurrentShield, damage);
    this->SetCurrentShield(this->m_CurrentShield - shieldDamage);
    Character::ApplyDamage(damage - shieldDamage);
}

bool Player::IsHealthLocked() const {
    return this->m_HealthLocked;
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
