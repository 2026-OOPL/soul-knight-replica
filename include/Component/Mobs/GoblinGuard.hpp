#ifndef MOB_HPP
#define MOB_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

#include "Component/AI.hpp"
#include "Component/Character/Character.hpp"

namespace {
    const std::vector<std::string> WALK_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_0.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_1.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_2.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_walk_3.png"
    };

    const std::vector<std::string> STAND_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_0.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_1.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_2.png",
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_stand_3.png"
    };

    const std::vector<std::string> DIE_SPRITE = {
        RESOURCE_DIR"/Mob/GoblinGuard/GoblinGuard_die.png"
    };
}

class GoblinGuard : public Character {
public:
    GoblinGuard(
        std::weak_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Character(
        STAND_SPRITE,
        WALK_SPRITE,
        DIE_SPRITE,
        4,
        CombatFaction::Enemy
    ) {
        m_TracePlayerTemp = tracePlayer;
        m_CollisionSystemTemp = collisionSystem;

        this->m_AI = std::make_shared<AI>(this, m_TracePlayerTemp, m_CollisionSystemTemp);

        this->m_PlayerSpeed = 0.05F;
    };

    glm::vec2 GetMoveIntent() const override;
    glm::vec2 GetFaceDirection() const override;

    void Update() override;
    
    std::shared_ptr<AI> m_AI = nullptr;
private:
    // 暫存建構子傳入的參數，供後續 InitAI 使用
    std::weak_ptr<Character> m_TracePlayerTemp;
    Collision::CollisionSystem* m_CollisionSystemTemp;

    glm::vec2 m_FacingDirection;
};

#endif
