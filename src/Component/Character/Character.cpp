#include <algorithm>
#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <utility>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include "Util/Animation.hpp"
#include "Util/Time.hpp"

#include "Component/Bullet.hpp"
#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Weapons/BadPistol.hpp"

namespace {

constexpr float kMaxCharacterMovementDeltaTimeMs = 50.0F;
constexpr float kCharacterImpulseDecayPerMs = 0.0012F;

Collision::CollisionFilter BuildCharacterFilter(CombatFaction faction) {
    Collision::CollisionFilter filter;
    filter.blocking = true;

    switch (faction) {
    case CombatFaction::Player:
        filter.layer = Collision::CollisionLayer::Player;
        filter.mask =
            Collision::ToMask(Collision::CollisionLayer::World) |
            Collision::ToMask(Collision::CollisionLayer::Prop) |
            Collision::ToMask(Collision::CollisionLayer::Enemy) |
            Collision::ToMask(Collision::CollisionLayer::EnemyProjectile) |
            Collision::ToMask(Collision::CollisionLayer::Trigger);
        break;

    case CombatFaction::Enemy:
        filter.layer = Collision::CollisionLayer::Enemy;
        filter.mask =
            Collision::ToMask(Collision::CollisionLayer::World) |
            Collision::ToMask(Collision::CollisionLayer::Prop) |
            Collision::ToMask(Collision::CollisionLayer::Player) |
            Collision::ToMask(Collision::CollisionLayer::PlayerProjectile) |
            Collision::ToMask(Collision::CollisionLayer::Trigger);
        break;

    case CombatFaction::Neutral:
    default:
        filter.layer = Collision::CollisionLayer::None;
        filter.mask = 0U;
        filter.blocking = false;
        break;
    }

    return filter;
}

Collision::CollisionBox BuildDefaultCharacterBodyBox(CombatFaction faction) {
    Collision::CollisionBox bodyBox;
    bodyBox.id = 0;
    bodyBox.type = Collision::CollisionBoxType::Body;
    bodyBox.size = {24.0F, 24.0F};
    bodyBox.filter = BuildCharacterFilter(faction);
    return bodyBox;
}

} // namespace

Character::Character(
    std::shared_ptr<Util::Animation> StandAnimation,
    std::shared_ptr<Util::Animation> WalkAnimation,
    std::shared_ptr<Util::Animation> DieAnimation,
    int zIndex,
    CombatFaction faction
) : GameObject(nullptr, zIndex),
    m_Faction(faction) {
    this->m_WalkAnimation = WalkAnimation;
    this->m_DieAnimation = DieAnimation;
    this->m_StandAnimation = StandAnimation;

    this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox(this->m_Faction));
    this->SetWeapon(std::make_shared<BadPistol>());
    this->SetDrawable(this->m_StandAnimation);
};

Character::Character(
    const std::vector<std::string>& StandSprite,
    const std::vector<std::string>& WalkSprite,
    const std::vector<std::string>& DieSprite,
    int zIndex,
    CombatFaction faction
) : GameObject(nullptr, zIndex),
    m_Faction(faction) {
    this->m_WalkAnimation = std::make_shared<Util::Animation>(
        WalkSprite, true, 20, true, 0, false
    );
    
    this->m_DieAnimation = std::make_shared<Util::Animation>(
        DieSprite, true, 20, true, 0, false
    );

    this->m_StandAnimation = std::make_shared<Util::Animation>(
        StandSprite, true, 20, true, 0, false
    );

    this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox(this->m_Faction));
    this->SetWeapon(std::make_shared<BadPistol>());
    this->SetDrawable(this->m_StandAnimation);
};

void Character::UpdateFaceDirection() {
    if (this->GetFaceDirection().x > 0 && this->m_AbsoluteTransform.scale.x < 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
        return;
    }

    if (this->GetFaceDirection().x < 0 && this->m_AbsoluteTransform.scale.x > 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
    }
}

void Character::SetSpriteTypeByMoveIntent(glm::vec2 moveIntent) {
    if (this->IsAttackVisualActive() && this->m_AttackAnimation != nullptr) {
        this->SetDrawable(this->m_AttackAnimation);
        return;
    }

    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        this->SetDrawable(this->m_StandAnimation);
        return;
    }

    this->SetDrawable(this->m_WalkAnimation);
}

void Character::SetAttackAnimation(std::shared_ptr<Util::Animation> attackAnimation) {
    this->m_AttackAnimation = std::move(attackAnimation);
}

void Character::TriggerAttackVisual(float durationMs) {
    this->m_AttackVisualEndTime = Util::Time::GetElapsedTimeMs() + durationMs;

    if (this->m_AttackAnimation != nullptr) {
        this->m_AttackAnimation->SetCurrentFrame(0);
        this->m_AttackAnimation->Play();
    }
}

bool Character::IsAttackVisualActive() const {
    return Util::Time::GetElapsedTimeMs() < this->m_AttackVisualEndTime;
}

Util::Transform Character::GetObjectTransform() const {
    return this->m_Transform;
}

std::shared_ptr<Weapon> Character::GetWeapon() {
    return m_Weapon;
}

void Character::SetWeapon(std::shared_ptr<Weapon> weapon) {
    if (this->m_Weapon == weapon) {
        return;
    }

    if (this->m_Weapon != nullptr) {
        this->RemoveChild(this->m_Weapon);
    }

    this->m_Weapon = std::move(weapon);

    if (this->m_Weapon != nullptr) {
        this->m_Weapon->SetProjectileFaction(this->m_Faction);
        this->AddChild(this->m_Weapon);
    }
}

int Character::GetCurrentHealth() const {
    return this->m_CurrentHealth;
}

int Character::GetMaxHealth() const {
    return this->m_MaxHealth;
}

void Character::SetCurrentHealth(int health) {
    this->m_CurrentHealth = std::clamp(health, 0, this->m_MaxHealth);
}

void Character::SetMaxHealth(int maxHealth) {
    this->m_MaxHealth = std::max(0, maxHealth);
    this->m_CurrentHealth = std::clamp(this->m_CurrentHealth, 0, this->m_MaxHealth);
}

CombatFaction Character::GetFaction() const {
    return this->m_Faction;
}

void Character::SetFaction(CombatFaction faction) {
    this->m_Faction = faction;
    this->SetCollisionFilter(BuildCharacterFilter(this->m_Faction));

    if (this->m_Weapon != nullptr) {
        this->m_Weapon->SetProjectileFaction(this->m_Faction);
    }
}

void Character::ApplyDamage(int damage) {
    if (damage <= 0) {
        return;
    }

    this->SetCurrentHealth(this->m_CurrentHealth - damage);
}

void Character::Heal(int amount) {
    if (amount <= 0) {
        return;
    }

    this->SetCurrentHealth(this->m_CurrentHealth + amount);
}

void Character::ApplyImpulse(const glm::vec2 &impulse) {
    if (glm::length(impulse) <= 0.0001F) {
        return;
    }

    this->m_ImpulseVelocity += impulse;
}

bool Character::IsDead() const {
    return this->m_CurrentHealth <= 0;
}

bool Character::CanBeDamagedBy(const Bullet &bullet) const {
    if (bullet.GetFaction() == CombatFaction::Neutral) {
        return false;
    }

    return bullet.GetFaction() != this->m_Faction;
}

void Character::Update() {
    const glm::vec2 moveIntent = this->GetMoveIntent();
    const float movementDeltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), kMaxCharacterMovementDeltaTimeMs);

    if (this->m_Weapon != nullptr) {
        this->m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
        this->m_Weapon->SetFacingDirection(this->GetFaceDirection());
    }

    this->SetSpriteTypeByMoveIntent(moveIntent);
    this->UpdateFaceDirection();

    if (moveIntent != glm::vec2(0, 0)) {
        m_LastMomentum = moveIntent;
    }

    const glm::vec2 frameDelta =
        moveIntent * this->m_PlayerSpeed * movementDeltaTimeMs +
        this->BuildImpulseDelta(movementDeltaTimeMs);

    if (glm::length(frameDelta) <= 0.0001F) {
        return;
    }

    if (this->m_CollisionResolver) {
        const Collision::MovementResult movementResult =
            this->m_CollisionResolver(*this, frameDelta);
        this->m_AbsoluteTransform.translation += movementResult.resolvedDelta;

        if (movementResult.blockedX) {
            this->m_ImpulseVelocity.x = 0.0F;
        }
        if (movementResult.blockedY) {
            this->m_ImpulseVelocity.y = 0.0F;
        }
        return;
    }

    this->m_AbsoluteTransform.translation += frameDelta;
}

glm::vec2 Character::GetCollisionOrigin() const {
    return this->GetAbsoluteTranslation();
}

const std::vector<Collision::CollisionBox> &Character::GetCollisionBoxes() const {
    return this->m_CollisionBoxes;
}

void Character::OnCollision(const Collision::CollisionSituation &situation) {
    if (this->IsDead() || situation.kind == Collision::CollisionSituationKind::Trigger) {
        return;
    }

    Bullet *bullet = dynamic_cast<Bullet *>(situation.other);
    if (bullet == nullptr || !this->CanBeDamagedBy(*bullet)) {
        return;
    }

    if (!bullet->TryRegisterImpact()) {
        return;
    }

    this->ApplyDamage(bullet->GetDamage());
    bullet->ApplyHitEffects(*this);
}

glm::vec2 Character::GetColliderSize() const {
    if (this->m_CollisionBoxes.empty()) {
        return {0.0F, 0.0F};
    }

    return this->m_CollisionBoxes.front().size;
}

void Character::SetColliderSize(const glm::vec2 &colliderSize) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox(this->m_Faction));
    }

    this->m_CollisionBoxes.front().size = colliderSize;
}

Collision::CollisionFilter Character::GetCollisionFilter() const {
    if (this->m_CollisionBoxes.empty()) {
        return BuildCharacterFilter(this->m_Faction);
    }

    return this->m_CollisionBoxes.front().filter;
}

void Character::SetCollisionFilter(const Collision::CollisionFilter &filter) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox(this->m_Faction));
    }

    this->m_CollisionBoxes.front().filter = filter;
}

Collision::AxisAlignedBox Character::GetCollisionBox() const {
    return this->GetCollisionBoxAt(this->GetCollisionOrigin());
}

Collision::AxisAlignedBox Character::GetCollisionBoxAt(const glm::vec2 &coordinate) const {
    if (this->m_CollisionBoxes.empty()) {
        return {};
    }

    const Collision::CollisionBox &bodyBox = this->m_CollisionBoxes.front();
    return Collision::CollisionSystem::BuildBox(
        coordinate + bodyBox.offset,
        bodyBox.size
    );
}

void Character::SetCollisionResolver(CollisionResolver collisionResolver) {
    this->m_CollisionResolver = std::move(collisionResolver);
}

glm::vec2 Character::BuildImpulseDelta(float deltaTimeMs) {
    if (glm::length(this->m_ImpulseVelocity) <= 0.0001F || deltaTimeMs <= 0.0F) {
        this->m_ImpulseVelocity = {0.0F, 0.0F};
        return {0.0F, 0.0F};
    }

    const glm::vec2 frameDelta = this->m_ImpulseVelocity * deltaTimeMs;
    const float nextSpeed = std::max(
        0.0F,
        glm::length(this->m_ImpulseVelocity) - kCharacterImpulseDecayPerMs * deltaTimeMs
    );

    if (nextSpeed <= 0.0001F) {
        this->m_ImpulseVelocity = {0.0F, 0.0F};
        return frameDelta;
    }

    this->m_ImpulseVelocity =
        glm::normalize(this->m_ImpulseVelocity) * nextSpeed;
    return frameDelta;
}
