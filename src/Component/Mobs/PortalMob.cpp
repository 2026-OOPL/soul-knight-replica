#include "Component/Mobs/PortalMob.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

#include <glm/geometric.hpp>

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/Mobs/RuinsGuard.hpp"
#include "Component/Mobs/RuinsTurret.hpp"

namespace {

const std::vector<std::string> kPortalStandSprite = {
    RESOURCE_DIR"/Mob/Portal/Portal_stand.png"
};

const std::vector<std::string> kPortalWalkSprite = {
    RESOURCE_DIR"/Mob/Portal/Portal_walk.png"
};

const std::vector<std::string> kPortalDieSprite = {
    RESOURCE_DIR"/Mob/Portal/Portal_die.png"
};

constexpr int kPortalHealth = 6;
constexpr int kMaxActiveSummons = 2;
constexpr float kInitialSummonDelayMs = 1200.0F;
constexpr float kMinSummonDelayMs = 3000.0F;
constexpr float kMaxSummonDelayMs = 5000.0F;
constexpr float kDefeatCooldownReductionMs = 1800.0F;
constexpr float kSummonVisualDurationMs = 220.0F;
constexpr float kSpawnMargin = 26.0F;
constexpr float kSpawnRadiusMin = 34.0F;
constexpr float kSpawnRadiusMax = 82.0F;
constexpr int kMaxSpawnAttempts = 18;
constexpr float kPi = 3.14159265358979323846F;

} // namespace

PortalMob::PortalMob(
    std::weak_ptr<Character> tracePlayer,
    Collision::CollisionSystem* collisionSystem
) : Mob(
        kPortalStandSprite,
        kPortalWalkSprite,
        kPortalDieSprite,
        tracePlayer,
        collisionSystem
    ),
    m_RandomEngine(std::random_device{}()),
    m_UnitDistribution(0.0F, 1.0F) {
    this->SetMaxHealth(kPortalHealth);
    this->SetCurrentHealth(this->GetMaxHealth());
    this->SetColliderSize({30.0F, 24.0F});
    this->m_PlayerSpeed = 0.0F;
    this->SetAttackAnimation(std::make_shared<Util::Animation>(
        kPortalWalkSprite,
        false,
        20,
        true,
        0,
        false
    ));
    this->m_NextSummonTime = Util::Time::GetElapsedTimeMs() + kInitialSummonDelayMs;
}

void PortalMob::Update() {
    if (this->IsDead()) {
        Character::Update();
        return;
    }

    if (this->m_AI != nullptr && this->m_AI->IsFrozen()) {
        Character::Update();
        return;
    }

    const std::shared_ptr<Character> target = this->GetTarget();
    if (target != nullptr && !target->IsDead()) {
        const glm::vec2 toTarget =
            target->GetAbsoluteTranslation() - this->GetAbsoluteTranslation();
        if (glm::length(toTarget) > 0.0001F) {
            this->m_FaceDirection = glm::normalize(toTarget);
        }
    }

    const std::size_t previousActiveCount = this->m_Summons.size();
    this->PruneSummons();
    if (this->m_Summons.size() < previousActiveCount) {
        const Util::ms_t now = Util::Time::GetElapsedTimeMs();
        this->m_NextSummonTime = std::min<Util::ms_t>(
            this->m_NextSummonTime,
            now + std::max(
                0.0F,
                static_cast<float>(this->m_NextSummonTime - now) -
                    kDefeatCooldownReductionMs
            )
        );
    }

    this->TrySummon();
    Character::Update();
}

glm::vec2 PortalMob::GetMoveIntent() const {
    return {0.0F, 0.0F};
}

glm::vec2 PortalMob::GetFaceDirection() const {
    return this->m_FaceDirection;
}

void PortalMob::UpdateWeaponPresentation() {
}

std::shared_ptr<Character> PortalMob::GetTarget() const {
    return this->m_TracePlayerTemp.lock();
}

void PortalMob::PruneSummons() {
    this->m_Summons.erase(
        std::remove_if(
            this->m_Summons.begin(),
            this->m_Summons.end(),
            [](const std::weak_ptr<Mob> &summon) {
                const std::shared_ptr<Mob> locked = summon.lock();
                return locked == nullptr || locked->IsDead();
            }
        ),
        this->m_Summons.end()
    );
}

void PortalMob::TrySummon() {
    if (this->m_MapSystem == nullptr ||
        this->m_Summons.size() >= kMaxActiveSummons ||
        Util::Time::GetElapsedTimeMs() < this->m_NextSummonTime) {
        return;
    }

    std::shared_ptr<Mob> summon = this->CreateSummon();
    if (summon == nullptr || !this->TryPlaceSummon(summon)) {
        this->ScheduleNextSummon(600.0F, 1000.0F);
        return;
    }

    std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room != nullptr) {
        room->AddMob(summon);
    }
    this->m_MapSystem->AddMob(summon);
    this->m_Summons.push_back(summon);
    this->TriggerAttackVisual(kSummonVisualDurationMs);
    this->ScheduleNextSummon(kMinSummonDelayMs, kMaxSummonDelayMs);
}

std::shared_ptr<Mob> PortalMob::CreateSummon() {
    const std::weak_ptr<Character> target = this->m_TracePlayerTemp;
    if (this->m_UnitDistribution(this->m_RandomEngine) < 0.5F) {
        return std::make_shared<RuinsGuard>(target, this->m_CollisionSystemTemp);
    }

    return std::make_shared<RuinsTurret>(target, this->m_CollisionSystemTemp);
}

bool PortalMob::TryPlaceSummon(const std::shared_ptr<Mob> &summon) {
    if (summon == nullptr || this->m_MapSystem == nullptr) {
        return false;
    }

    const std::shared_ptr<BaseRoom> room = this->m_MapSystem->GetCurrentRoom();
    if (room == nullptr) {
        return false;
    }

    const glm::vec2 roomCenter = room->GetAbsoluteTranslation();
    const glm::vec2 roomHalfSize = room->GetAreaSize() * 0.5F;
    const glm::vec2 summonHalfSize = summon->GetColliderSize() * 0.5F;
    const float left = roomCenter.x - roomHalfSize.x + kSpawnMargin + summonHalfSize.x;
    const float right = roomCenter.x + roomHalfSize.x - kSpawnMargin - summonHalfSize.x;
    const float bottom = roomCenter.y - roomHalfSize.y + kSpawnMargin + summonHalfSize.y;
    const float top = roomCenter.y + roomHalfSize.y - kSpawnMargin - summonHalfSize.y;

    if (left >= right || bottom >= top) {
        return false;
    }

    const glm::vec2 origin = this->GetAbsoluteTranslation();
    for (int attempt = 0; attempt < kMaxSpawnAttempts; ++attempt) {
        const float angle = this->m_UnitDistribution(this->m_RandomEngine) * 2.0F * kPi;
        const float radius =
            kSpawnRadiusMin +
            (kSpawnRadiusMax - kSpawnRadiusMin) *
                this->m_UnitDistribution(this->m_RandomEngine);
        glm::vec2 candidate = {
            origin.x + std::cos(angle) * radius,
            origin.y + std::sin(angle) * radius
        };
        candidate.x = std::clamp(candidate.x, left, right);
        candidate.y = std::clamp(candidate.y, bottom, top);

        summon->SetAbsoluteTranslation(candidate);
        if (room->IsPlayerInside(candidate) &&
            this->m_MapSystem->CanOccupy(*summon, candidate)) {
            return true;
        }
    }

    return false;
}

void PortalMob::ScheduleNextSummon(float minMs, float maxMs) {
    const float t = this->m_UnitDistribution(this->m_RandomEngine);
    this->m_NextSummonTime =
        Util::Time::GetElapsedTimeMs() + minMs + (maxMs - minMs) * t;
}
