#ifndef MOB_HPP
#define MOB_HPP

#include <memory>
#include <vector>

#include "Common/CombatFaction.hpp"
#include "Component/AI/AI.hpp"
#include "Component/Character/Character.hpp"

class MapSystem;

class Mob : public Character {
public:
    Mob(
        const std::vector<std::string>& StandAnimation,
        const std::vector<std::string>& WalkAnimation,
        const std::vector<std::string>& DieAnimation,
        std::shared_ptr<Character> tracePlayer,
        Collision::CollisionSystem* collisionSystem
    ) : Character(
        StandAnimation,
        WalkAnimation,
        DieAnimation,
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

    void Initialize(MapSystem* mapSystem);
    
protected:
    std::shared_ptr<Character> m_TracePlayerTemp;
    Collision::CollisionSystem* m_CollisionSystemTemp;

    glm::vec2 m_FacingDirection;

    MapSystem* m_MapSystem;
};

#endif
