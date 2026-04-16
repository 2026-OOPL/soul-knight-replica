#include <algorithm>
#include <cmath>
#include <utility>

#include <glm/vec2.hpp>

#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

#include "Component/Bullet.hpp"

namespace {

Collision::CollisionFilter BuildBulletFilterForFaction(CombatFaction faction) {
    Collision::CollisionFilter filter;

    switch (faction) {
        case CombatFaction::Player:
            filter.layer = Collision::CollisionLayer::PlayerProjectile;
            filter.mask =
                Collision::ToMask(Collision::CollisionLayer::World) |
                Collision::ToMask(Collision::CollisionLayer::Prop) |
                Collision::ToMask(Collision::CollisionLayer::Enemy) |
                Collision::ToMask(Collision::CollisionLayer::Trigger);
            break;

        case CombatFaction::Enemy:
            filter.layer = Collision::CollisionLayer::EnemyProjectile;
            filter.mask =
                Collision::ToMask(Collision::CollisionLayer::World) |
                Collision::ToMask(Collision::CollisionLayer::Prop) |
                Collision::ToMask(Collision::CollisionLayer::Player) |
                Collision::ToMask(Collision::CollisionLayer::Trigger);
            break;

        case CombatFaction::Neutral:
        default:
            filter.layer = Collision::CollisionLayer::PlayerProjectile;
            filter.mask =
                Collision::ToMask(Collision::CollisionLayer::World) |
                Collision::ToMask(Collision::CollisionLayer::Prop) |
                Collision::ToMask(Collision::CollisionLayer::Trigger);
            break;
    }

    filter.blocking = false;
    return filter;
}

Collision::CollisionBox BuildDefaultBulletBodyBox(CombatFaction faction) {
    Collision::CollisionBox box;
    box.id = 0;
    box.type = Collision::CollisionBoxType::Body;
    box.size = {12.0F, 12.0F};
    box.filter = BuildBulletFilterForFaction(faction);
    return box;
}

} // namespace

Bullet::Bullet(
    std::shared_ptr<Util::Animation> animation,
    glm::vec2 cooridinate,
    glm::vec2 momentum,
    int zIndex,
    int damage,
    CombatFaction faction
) : Util::GameObject(nullptr, zIndex) {
    this->m_Animation = animation;
    this->m_Momentum = momentum;
    this->m_Damage = std::max(0, damage);
    this->m_Faction = faction;

    this->m_AbsoluteTransform.translation = cooridinate;
    this->m_CollisionBoxes.push_back(BuildDefaultBulletBodyBox(this->m_Faction));

    this->SetDrawable(this->m_Animation);

    // Set initial pointing direction
    float rotation = atan2(this->m_Momentum.y, this->m_Momentum.x);

    if (fabs(rotation) > M_PI / 2.0f) {
        this->SetAbsoluteScale(
            this->GetAbsoluteScale() * glm::vec2(1, -1)
        );
    }

    this->SetAbsoluteRotation(rotation);
}

Bullet::Bullet(
    const std::vector<std::string>& sprite,
    glm::vec2 cooridinate,
    glm::vec2 momentum,
    int zIndex,
    int damage,
    CombatFaction faction
) : Bullet(
    std::make_shared<Util::Animation>(sprite, true, 20),
    cooridinate,
    momentum,
    zIndex,
    damage,
    faction
) {}

Util::Transform Bullet::GetObjectTransform() const {
    return this->m_Transform;
}

void Bullet::Update() {
    const float movementDeltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), 10.0F);

    const glm::vec2 frameDelta =
        m_Momentum * 0.5F * movementDeltaTimeMs;

    if (this->m_CollisionResolver) {
        const Collision::MovementResult movementResult =
            this->m_CollisionResolver(*this, frameDelta);

        this->m_AbsoluteTransform.translation += movementResult.resolvedDelta;

        if (movementResult.blockedX || movementResult.blockedY) {
            this->RequestDestroy();
        }
        return;
    }

    this->m_AbsoluteTransform.translation += frameDelta;
}

glm::vec2 Bullet::GetCollisionOrigin() const {
    return this->GetAbsoluteTranslation();
}

const std::vector<Collision::CollisionBox> &Bullet::GetCollisionBoxes() const {
    return this->m_CollisionBoxes;
}

void Bullet::OnCollision(const Collision::CollisionSituation &situation) {
    if (situation.kind == Collision::CollisionSituationKind::Trigger) {
        return;
    }

    this->TryRegisterImpact();
}

glm::vec2 Bullet::GetColliderSize() const {
    if (this->m_CollisionBoxes.empty()) {
        return {0.0F, 0.0F};
    }

    return this->m_CollisionBoxes.front().size;
}

void Bullet::SetColliderSize(const glm::vec2 &colliderSize) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultBulletBodyBox(this->m_Faction));
    }

    this->m_CollisionBoxes.front().size = colliderSize;
}

void Bullet::SetCollisionFilter(const Collision::CollisionFilter &filter) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultBulletBodyBox(this->m_Faction));
    }

    this->m_CollisionBoxes.front().filter = filter;
}

void Bullet::SetCollisionResolver(CollisionResolver collisionResolver) {
    this->m_CollisionResolver = std::move(collisionResolver);
}

int Bullet::GetDamage() const {
    return this->m_Damage;
}

void Bullet::SetDamage(int damage) {
    this->m_Damage = std::max(0, damage);
}

CombatFaction Bullet::GetFaction() const {
    return this->m_Faction;
}

void Bullet::SetFaction(CombatFaction faction) {
    this->m_Faction = faction;
    this->SetCollisionFilter(BuildBulletFilterForFaction(this->m_Faction));
}

bool Bullet::HasRegisteredImpact() const {
    return this->m_ImpactRegistered;
}

bool Bullet::TryRegisterImpact() {
    if (this->m_ImpactRegistered) {
        return false;
    }

    this->m_ImpactRegistered = true;
    this->RequestDestroy();
    return true;
}

bool Bullet::IsDestroyRequested() const {
    return this->m_DestroyRequested;
}

void Bullet::RequestDestroy() {
    this->m_DestroyRequested = true;
}
