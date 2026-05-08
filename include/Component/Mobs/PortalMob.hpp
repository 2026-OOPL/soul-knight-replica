#ifndef PORTAL_MOB_HPP
#define PORTAL_MOB_HPP

#include <memory>
#include <random>
#include <vector>

#include <glm/vec2.hpp>

#include "Component/Character/Character.hpp"
#include "Component/Mobs/Mob.hpp"
#include "Util/Time.hpp"

class PortalMob : public Mob {
public:
    PortalMob(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    );

    void Update() override;
    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

protected:
    void UpdateWeaponPresentation() override;

private:
    std::shared_ptr<Character> GetTarget() const;
    void PruneSummons();
    void TrySummon();
    std::shared_ptr<Mob> CreateSummon();
    bool TryPlaceSummon(const std::shared_ptr<Mob> &summon);
    void ScheduleNextSummon(float minMs, float maxMs);

    std::vector<std::weak_ptr<Mob>> m_Summons;
    Util::ms_t m_NextSummonTime = 0;
    glm::vec2 m_FaceDirection = {1.0F, 0.0F};
    std::mt19937 m_RandomEngine;
    std::uniform_real_distribution<float> m_UnitDistribution;
};

#endif
