#include "Util/Logger.hpp"
#include "Util/Transform.hpp"
#include <Component/Camera/TraceCamera.hpp>
#include <Component/IMapObject.hpp>
#include <glm/fwd.hpp>

Util::Transform TraceCamera::GetTransformByCamera(std::shared_ptr<IMapObject> object) {
    // The view center of this camera 
    glm::vec2 cameraCooridinate = this->GetCooridinate();
    glm::vec2 objectCooridinate = object->GetCooridinate();

    return {
        objectCooridinate - cameraCooridinate,
        GetRotation(),
        GetScale()
    };
}

glm::vec2 TraceCamera::GetTargetCooridinate() {
    return m_Target->GetCooridinate();
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