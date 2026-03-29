#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Util/Transform.hpp"

Util::Transform TraceCamera::GetTransformByCamera(std::shared_ptr<MapObject> object) {
    Util::Transform objectAbs = object->GetAbsoluteTransform();
    Util::Transform cameraAbs = this->m_Transform;

    // 1. 計算世界空間位移 (World Delta)
    glm::vec2 worldDelta = objectAbs.translation - cameraAbs.translation;

    // 2. 處理反向旋轉 (Inverse Rotation)
    // 假設相機轉 30 度，畫面內容就要轉 -30 度
    float cosTheta = std::cos(-cameraAbs.rotation);
    float sinTheta = std::sin(-cameraAbs.rotation);

    glm::vec2 rotatedTranslation = {
        worldDelta.x * cosTheta - worldDelta.y * sinTheta,
        worldDelta.x * sinTheta + worldDelta.y * cosTheta
    };

    // 3. 處理相機縮放 (Scale as Zoom Factor)
    // 當 cameraAbs.scale 為 2 時，位移與大小皆放大兩倍
    glm::vec2 finalTranslation = {
        rotatedTranslation.x * cameraAbs.scale.x,
        rotatedTranslation.y * cameraAbs.scale.y
    };

    glm::vec2 finalScale = {
        objectAbs.scale.x * cameraAbs.scale.x,
        objectAbs.scale.y * cameraAbs.scale.y
    };

    // 4. 回傳變換結果
    return {
        finalTranslation,
        objectAbs.rotation - cameraAbs.rotation, // 相對旋轉通常仍為相減
        finalScale
    };
}

glm::vec2 TraceCamera::GetTargetCooridinate() {
    return m_Target->GetAbsoluteTransform().translation;
}

void TraceCamera::Update() {
    this->m_TargetCooridinate = this->GetTargetCooridinate();

    if (m_Curve) {
        glm::vec2 moveTarget = m_Curve->ApplyCurve(this->GetCooridinate(), m_TargetCooridinate);

        this->SetCooridinate(moveTarget);
    } else {
        this->SetCooridinate(m_TargetCooridinate);
    }
}
