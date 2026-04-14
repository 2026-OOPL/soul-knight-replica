#ifndef AI_HPP
#define AI_HPP

#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <random>

#include "Component/Collision/CollisionSystem.hpp"
#include "Component/IStateful.hpp"
#include "Component/Player/Player.hpp"
#include "Util/Time.hpp"

enum class Status {
    WANDER, 
    PURSUIT,
    STOPANDATTACK
};

class AI : public IStateful {
public:
    AI(
        Character* owner,
        std::weak_ptr<Character> target,
        Collision::CollisionSystem* collision
    ) {
      this->m_Owner = owner;
      this->m_TracePlayer = target;
      this->m_CollisionSystem = collision;

      gen = std::mt19937(this->rand());
      distr = std::uniform_real_distribution<float>(0.0, 1.0);
    };

    ~AI() override = default;
    
    void Update() override;
    
    bool IsArriveDestinaton();

    bool GetAttackTrigger();
    glm::vec2 GetMoveDirection();
    glm::vec2 GetFaceDirection();

    void Freeze();
    void UnFreeze();

protected:
    Character* m_Owner;       // AI 控制的本體 (Mob)
    std::weak_ptr<Character> m_TracePlayer;
    Collision::CollisionSystem* m_CollisionSystem;

    glm::vec2 m_DesiredDirection;
    glm::vec2 m_LastMoveDirection;

    Status m_Status = Status::WANDER;

private:
    // 觸鬚避障相關方法
    glm::vec2 ApplyObstacleAvoidance(const glm::vec2& currentPos, const glm::vec2& desiredDir);
    
    // 假定的碰撞檢測函式：你需要將這個函式連接到你的地圖/碰撞系統
    // 傳入一個世界座標，判斷該點是否有牆壁或障礙物
    bool IsPointBlocked(const glm::vec2& point); 

    void UpdateDesiredDirection();

    std::random_device rand; 
    
    std::mt19937 gen;

    std::uniform_real_distribution<float> distr;
    glm::vec2 CalculateStopAndAttack(glm::vec2 ownerPos, glm::vec2 targetPos);

    Util::ms_t m_LastUpdateTime = 0;

    glm::vec2 m_LastPosition;

    bool m_Freezed = false;
};

#endif //
