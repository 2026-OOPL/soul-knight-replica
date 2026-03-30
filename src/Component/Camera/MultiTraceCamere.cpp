#include <glm/vec2.hpp>

#include "Util/Transform.hpp"
#include "Common/MapObject.hpp"
#include "Component/Camera/MultiTraceCamere.hpp"

Util::Transform MultiTraceCamera::GetTransformByCamera(std::shared_ptr<MapObject> object) {
    Util::Transform objectTransform = object->GetAbsoluteTransform();

    // The view center of this camera 
    glm::vec2 cameraCooridinate = this->m_TargetCooridinate;
    glm::vec2 objectCooridinate = objectTransform.translation;

    return {
        objectCooridinate - cameraCooridinate,
        0,
        {1, 1}
    };
}

glm::vec2 MultiTraceCamera::GetTargetCooridinate() {
    int targetCount = m_Target.size();
    glm::vec2 targetCooridinate = {0, 0};

    for (auto const & i : m_Target) {
        Util::Transform objectTransform = i->GetAbsoluteTransform();
        targetCooridinate += objectTransform.translation / glm::vec2(targetCount, targetCount);  
    }

    return targetCooridinate;
}

void MultiTraceCamera::Update() {
    this->m_TargetCooridinate = this->GetTargetCooridinate();
}