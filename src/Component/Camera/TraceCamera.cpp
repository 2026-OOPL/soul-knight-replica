#include "Util/Logger.hpp"
#include "Util/Transform.hpp"
#include <Component/Camera/TraceCamera.hpp>
#include <Component/IMapObject.hpp>
#include <glm/fwd.hpp>

Util::Transform TraceCamera::GetTransformByCamera(std::shared_ptr<IMapObject> object) {
    Util::Transform objectAbs = object->GetAbsoluteTransform();
    Util::Transform cameraAbs = this->m_Transform;

    // 1. 計算世界空間中的位移向量
    glm::vec2 worldDelta = objectAbs.translation - cameraAbs.translation;

    // 2. 處理旋轉：物件必須繞著相機原點做反向旋轉
    // 這裡假設 rotation 是弧度，且我們要套用的是相機旋轉的逆矩陣
    float cosTheta = std::cos(-cameraAbs.rotation);
    float sinTheta = std::sin(-cameraAbs.rotation);

    glm::vec2 relativeTranslation = {
        worldDelta.x * cosTheta - worldDelta.y * sinTheta,
        worldDelta.x * sinTheta + worldDelta.y * cosTheta
    };

    // 3. 計算相對旋轉與縮放
    // 旋轉通常是角度相減，縮放則是比例相除（看你的應用場景）
    return {
        relativeTranslation,
        objectAbs.rotation - cameraAbs.rotation,
        objectAbs.scale * cameraAbs.scale // 通常局部縮放是相除，確保物件大小受相機倍率影響
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