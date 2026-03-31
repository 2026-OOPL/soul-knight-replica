#include <algorithm>
#include <cmath>
#include <utility>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include "Util/Animation.hpp"
#include "Util/Time.hpp"

#include "Component/Character/Character.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Weapons/BadPistol.hpp"

namespace {

constexpr float kMaxCharacterMovementDeltaTimeMs = 50.0F;

Collision::CollisionFilter BuildDefaultCharacterFilter() {
    Collision::CollisionFilter filter;
    filter.layer = Collision::CollisionLayer::Enemy;
    filter.mask =
        Collision::ToMask(Collision::CollisionLayer::World) |
        Collision::ToMask(Collision::CollisionLayer::Prop) |
        Collision::ToMask(Collision::CollisionLayer::Player) |
        Collision::ToMask(Collision::CollisionLayer::PlayerProjectile) |
        Collision::ToMask(Collision::CollisionLayer::Trigger);
    filter.blocking = true;
    return filter;
}

Collision::CollisionBox BuildDefaultCharacterBodyBox() {
    Collision::CollisionBox bodyBox;
    bodyBox.id = 0;
    bodyBox.type = Collision::CollisionBoxType::Body;
    bodyBox.size = {24.0F, 24.0F};
    bodyBox.filter = BuildDefaultCharacterFilter();
    return bodyBox;
}

} // namespace

Character::Character(
    std::shared_ptr<Util::Animation> StandAnimation,
    std::shared_ptr<Util::Animation> WalkAnimation,
    std::shared_ptr<Util::Animation> DieAnimation,
    int zIndex
) : GameObject(nullptr, zIndex) {
    this->m_WalkAnimation = WalkAnimation;
    this->m_DieAnimation = DieAnimation;
    this->m_StandAnimation = StandAnimation;

    this->m_Weapon = std::make_shared<BadPistol>();
    this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox());

    this->AddChild(m_Weapon);
    this->SetDrawable(this->m_StandAnimation);
};

Character::Character(
    const std::vector<std::string>& StandSprite,
    const std::vector<std::string>& WalkSprite,
    const std::vector<std::string>& DieSprite,
    int zIndex
) : GameObject(nullptr, zIndex) {
    this->m_WalkAnimation = std::make_shared<Util::Animation>(
        WalkSprite, true, 20, true, 0
    );
    
    this->m_DieAnimation = std::make_shared<Util::Animation>(
        DieSprite, true, 20, true, 0
    );

    this->m_StandAnimation = std::make_shared<Util::Animation>(
        StandSprite, true, 20, true, 0
    );

    this->m_Weapon = std::make_shared<BadPistol>();
    this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox());

    this->AddChild(m_Weapon);
    this->SetDrawable(this->m_StandAnimation);
};

void Character::SetLookDirectionByMoveIntent(glm::vec2 moveIntent) {
    if (moveIntent.x > 0 && this->m_AbsoluteTransform.scale.x < 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
        return;
    }

    if (moveIntent.x < 0 && this->m_AbsoluteTransform.scale.x > 0) {
        this->m_AbsoluteTransform.scale.x *= -1;
    }
}

void Character::SetSpriteTypeByMoveIntent(glm::vec2 moveIntent) {
    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        this->SetDrawable(this->m_StandAnimation);
        return;
    }

    this->SetDrawable(this->m_WalkAnimation);
}

Util::Transform Character::GetObjectTransform() const {
    return this->m_Transform;
}

std::shared_ptr<Weapon> Character::GetWeapon() {
    return m_Weapon;
}

void Character::SetWeapon(std::shared_ptr<Weapon> weapon) {
    this->m_Weapon = weapon;
}

glm::vec2 Character::GetMoveIntent() const {
    return glm::vec2(0, 0);
}

void Character::Update() {
    const glm::vec2 moveIntent = this->GetMoveIntent();

    // Set weapon position
    if (this->m_Weapon != nullptr && moveIntent != glm::vec2(0, 0)) {
        m_Weapon->SetAnchorPoint(this->GetAbsoluteTranslation());
        m_Weapon->SetFacingDirection(moveIntent);
    }

    this->SetSpriteTypeByMoveIntent(moveIntent);

    if (moveIntent != glm::vec2(0, 0)) {
        this->SetLookDirectionByMoveIntent(moveIntent);
        m_LastMomentum = moveIntent;
    }

    if (moveIntent == glm::vec2(0.0F, 0.0F)) {
        return;
    }

    const float movementDeltaTimeMs =
        std::min(Util::Time::GetDeltaTimeMs(), kMaxCharacterMovementDeltaTimeMs);
    const glm::vec2 frameDelta =
        moveIntent * this->m_PlayerSpeed * movementDeltaTimeMs;

    if (this->m_CollisionResolver) {
        const Collision::MovementResult movementResult =
            this->m_CollisionResolver(*this, frameDelta);
        this->m_AbsoluteTransform.translation += movementResult.resolvedDelta;
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
    (void)situation;
}

glm::vec2 Character::GetColliderSize() const {
    if (this->m_CollisionBoxes.empty()) {
        return {0.0F, 0.0F};
    }

    return this->m_CollisionBoxes.front().size;
}

void Character::SetColliderSize(const glm::vec2 &colliderSize) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox());
    }

    this->m_CollisionBoxes.front().size = colliderSize;
}

Collision::CollisionFilter Character::GetCollisionFilter() const {
    if (this->m_CollisionBoxes.empty()) {
        return BuildDefaultCharacterFilter();
    }

    return this->m_CollisionBoxes.front().filter;
}

void Character::SetCollisionFilter(const Collision::CollisionFilter &filter) {
    if (this->m_CollisionBoxes.empty()) {
        this->m_CollisionBoxes.push_back(BuildDefaultCharacterBodyBox());
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
