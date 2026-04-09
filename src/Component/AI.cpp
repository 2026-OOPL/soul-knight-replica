#include "Component/AI.hpp"
#include "Component/Collision/CollisionSystem.hpp"
#include "Component/Collision/CollisionTypes.hpp"
#include "Util/Transform.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <glm/geometric.hpp> // 為了使用 glm::distance

void AI::Update() {
    
}

void AI::UpdateStatus() {
    std::shared_ptr<Character> player = m_TracePlayer.lock();

    if (!player || !m_Owner) {
        return;
    }

    Util::Transform playerTransform = player->GetAbsoluteTransform();
    Util::Transform ownerTransform = m_Owner->GetAbsoluteTransform();

    float dist = glm::distance(ownerTransform.translation, playerTransform.translation);
    
    if (dist > 300.0f) {
        m_Status = Status::WANDER;
    } 
    else if (dist > 100.0f) {
        m_Status = Status::PURSUIT;
    } 
    else {
        m_Status = Status::STOPANDATTACK;
    }
}

glm::vec2 AI::GetMoveDirection() {
    std::shared_ptr<Character> player = m_TracePlayer.lock();
    
    if (!player || !m_Owner) return glm::vec2(0.0f);

    glm::vec2 ownerPos = m_Owner->GetAbsoluteTranslation();
    glm::vec2 playerPos = player->GetAbsoluteTranslation();
    
    // 1. 計算原始渴望移動的方向 (正規化)
    glm::vec2 desiredDir = glm::normalize(playerPos - ownerPos);

    // 2. 透過觸鬚避障修正方向
    glm::vec2 finalDir = ApplyObstacleAvoidance(ownerPos, desiredDir);

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