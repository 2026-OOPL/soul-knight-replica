#include "Component/AI.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Collision/CollisionTypes.hpp"
#include "Util/Transform.hpp"
#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <glm/fwd.hpp>
#include <iterator>
#include <memory>
#include <glm/geometric.hpp> // 為了使用 glm::distance
#include <random>
#include "Util/Logger.hpp"

void AI::Update() {
    std::shared_ptr<Character> player = m_TracePlayer.lock();

    if (!player || !m_Owner) {
        return;
    }

    Util::Transform playerTransform = player->GetAbsoluteTransform();
    Util::Transform ownerTransform = m_Owner->GetAbsoluteTransform();

    float dist = glm::distance(ownerTransform.translation, playerTransform.translation);
    
    if (dist > 300.0f) {
        m_Status = Status::WANDER;
    } else if (dist > 100.0f) {
        m_Status = Status::PURSUIT;
    } else {
        m_Status = Status::STOPANDATTACK;
    }

    Util::ms_t updateInterval = Util::Time::GetElapsedTimeMs() - m_LastUpdateTime;
    if ((IsArriveDestinaton() || updateInterval > 3000) && updateInterval > 1500) {
        this->m_LastUpdateTime = Util::Time::GetElapsedTimeMs();
        this->UpdateDesiredDirection();
    }

    m_LastPosition = m_Owner->GetAbsoluteTranslation();
}

bool AI::IsArriveDestinaton() {
    if ( glm::distance(m_LastPosition, m_Owner->GetAbsoluteTranslation()) < 0.005F) {
        return true;
    }
    
    // LOG_INFO(m_LastMoveDirection);
    if (glm::distance(m_Owner->GetAbsoluteTransform().translation, m_DesiredDirection) < 1.0F) {
        return true;
    }

    return false;
}

void AI::UpdateDesiredDirection() {
    std::shared_ptr<Character> target = m_TracePlayer.lock();

    switch (m_Status) {
        case Status::WANDER:
            m_DesiredDirection = this->m_Owner->GetAbsoluteTranslation() + 
                glm::vec2(1, 0);
            break;

        case Status::PURSUIT:
            m_DesiredDirection = target->GetAbsoluteTranslation();
            break;

        case Status::STOPANDATTACK:
            m_DesiredDirection = this->CalculateStopAndAttack(
                this->m_Owner->GetAbsoluteTranslation(),
                target->GetAbsoluteTranslation()
            );

            break;
    }
}

glm::vec2 AI::CalculateStopAndAttack(glm::vec2 ownerPos, glm::vec2 targetPos) {
    // 1. 計算由 target 指向 owner 的單位向量
    glm::vec2 toOwner = ownerPos - targetPos;
    
    if (glm::length(toOwner) < 0.01f) {
        toOwner = glm::vec2(1.0f, 0.0f); // 防呆處理，避免位置完全重疊導致除以零
    } else {
        toOwner = glm::normalize(toOwner);
    }

    // 2. 在 owner 那側的半圓隨機取一個角度 (-90度 到 90度)
    float randomAngle = (this->distr(this->gen) - 0.5f) * M_PI / 1.5F;

    // 3. 將向量旋轉該隨機角度
    float s = std::sin(randomAngle);
    float c = std::cos(randomAngle);
    glm::vec2 rotatedDir = glm::vec2(toOwner.x * c - toOwner.y * s, toOwner.x * s + toOwner.y * c);

    // 4. 計算並回傳半圓上的目標座標
    float radius = 50.0f + 20.0f * this->distr(this->gen);
    return targetPos + rotatedDir * radius;
}

glm::vec2 AI::GetMoveDirection() {
    if (m_Freezed) {
        return glm::vec2(0.0f);
    }

    std::shared_ptr<Character> player = m_TracePlayer.lock();
    
    if (!player || !m_Owner) return glm::vec2(0.0f);

    glm::vec2 ownerPos = m_Owner->GetAbsoluteTranslation();
    
    // 1. 計算原始渴望移動的方向 (正規化)
    glm::vec2 desiredDir = glm::normalize(m_DesiredDirection - ownerPos);

    if (glm::distance(m_DesiredDirection, ownerPos) < 0.5F) {
        return glm::vec2(0);
    }

    // 2. 透過觸鬚避障修正方向
    glm::vec2 finalDir = ApplyObstacleAvoidance(ownerPos, desiredDir);

    m_LastMoveDirection = finalDir;

    return finalDir;
}

glm::vec2 AI::ApplyObstacleAvoidance(const glm::vec2& currentPos, const glm::vec2& desiredDir) {
    // 如果原本就沒有要移動，就不用避障
    if (glm::length(desiredDir) < 0.01f) return desiredDir;

    // 旋轉向量的輔助 Lambda
    auto rotateVec = [](const glm::vec2& v, float angle) {
        float s = sin(angle);
        float c = cos(angle);
        return glm::vec2(v.x * c - v.y * s, v.x * s + v.y * c);
    };

    // 定義觸鬚的角度與長度
    const float sideAngle = glm::radians(30.0f); // 左右觸鬚偏角 30 度
    const float centerLength = 60.0f;            // 中間觸鬚長度
    const float sideLength = 40.0f;              // 左右觸鬚長度

    // 計算三根觸鬚的方向向量
    glm::vec2 centerWhisker = desiredDir * centerLength;
    glm::vec2 leftWhisker = rotateVec(desiredDir, sideAngle) * sideLength;
    glm::vec2 rightWhisker = rotateVec(desiredDir, -sideAngle) * sideLength;

    glm::vec2 avoidanceForce(0.0f); // 最終要疊加的迴避力
    int hitCount = 0;

    // 檢測一根觸鬚上的 3 個點 (利用迴圈)
    auto checkWhisker = [&](const glm::vec2& whiskerBase, float length, float avoidWeight, const glm::vec2& avoidDir) {
        for (int i = 1; i <= 3; ++i) {
            float fraction = i / 3.0f; // 分別檢測 33%, 66%, 100% 的位置
            glm::vec2 checkPoint = currentPos + glm::normalize(whiskerBase) * (length * fraction);
            
            if (IsPointBlocked(checkPoint)) {
                // 越靠近怪物的點碰到，排斥力越強 (1.0 到 3.0)
                float strength = (4.0f - i) * avoidWeight; 
                avoidanceForce += avoidDir * strength;
                hitCount++;
                break; // 只要這根觸鬚的某個點碰到了，就不再檢查更遠的點
            }
        }
    };

    // 中間碰到 -> 強烈推向側面 (這裡選擇推向法線方向)
    checkWhisker(centerWhisker, centerLength, 2.0f, glm::vec2(-desiredDir.y, desiredDir.x)); 
    
    // 左邊碰到 -> 推向右邊
    checkWhisker(leftWhisker, sideLength, 1.0f, rotateVec(desiredDir, -sideAngle)); 
    
    // 右邊碰到 -> 推向左邊
    checkWhisker(rightWhisker, sideLength, 1.0f, rotateVec(desiredDir, sideAngle)); 

    // 如果有產生迴避力，將原本的方向與迴避力混合
    if (hitCount > 0) {
        glm::vec2 steeredDir = glm::normalize(desiredDir + avoidanceForce);
        return steeredDir;
    }

    return desiredDir; // 沒碰到任何東西，照原方向走
}

bool AI::IsPointBlocked(const glm::vec2& point) {

    if (!m_CollisionSystem) {
        return false;
    }

    // 1. 建立一個極小的虛擬碰撞盒 (AABB)，代表觸鬚的探測點
    // 注意：這裡假設 Collision::AxisAlignedBox 能夠設定大小與過濾器
    // 如果你的 AxisAlignedBox 沒有 filter 屬性，請依照你實際的結構設定
    Collision::AxisAlignedBox dummyBox;
    // 假設需要設定大小，設為極小值
    dummyBox.center = point; 
    dummyBox.size = glm::vec2(1 ,1);

    // 2. 給予一個極微小的測試位移
    glm::vec2 testDelta(0.01f, 0.0f);

    // 3. 呼叫系統的移動解算
    // 這裡的參數請依照你 CollisionSystem::ResolveMovement 實際的宣告進行調整
    // 假設它回傳類似 Collision::MovementResult 的結構
    auto result = m_CollisionSystem->ResolveMovement(dummyBox, testDelta); // 或可能需要傳入 point

    // 4. 如果在微小移動中被擋住了，代表該點本身就位於牆壁(或障礙物)內部
    bool isBlocked = (result.blockedX || result.blockedY);

    return isBlocked; 
}

glm::vec2 AI::GetFaceDirection() {
    if (m_Freezed) {
        return glm::vec2(0.0f);
    }

    switch (m_Status) {
        case Status::WANDER:
            return m_LastMoveDirection;

        case Status::STOPANDATTACK:
        case Status::PURSUIT:
            return m_TracePlayer.lock()->GetAbsoluteTranslation() - m_Owner->GetAbsoluteTranslation();
    }

    return glm::vec2(0.0f);
}

void AI::Freeze() {
    m_Freezed = true;
}

void AI::UnFreeze() {
    m_Freezed = false;
}

bool AI::GetAttackTrigger() {
    if (m_Freezed) {
        return false;
    }

    if (Util::Time::GetElapsedTimeMs() - m_LastShotTime < 500) {
        return false;
    }
    
    m_LastShotTime = Util::Time::GetElapsedTimeMs();


    switch (m_Status) {
        case Status::WANDER:
        case Status::PURSUIT:
            return false;

        case Status::STOPANDATTACK:
            return true;
    }
}