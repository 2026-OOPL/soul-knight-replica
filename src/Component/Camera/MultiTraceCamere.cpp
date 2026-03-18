#include "Util/Transform.hpp"
#include <Component/Camera/MultiTraceCamere.hpp>
#include <Component/IMapObject.hpp>
#include <glm/fwd.hpp>

Util::Transform MultiTraceCamera::GetTargetCooridinate(std::shared_ptr<IMapObject> object) {
    
    // The view center of this camera 
    glm::vec2 cameraCooridinate = this->m_TargetCooridinate;
    glm::vec2 objectCooridinate = object->GetCooridinate();

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
        targetCooridinate += i->GetCooridinate() / glm::vec2(targetCount, targetCount);  
    }

    return targetCooridinate;
}

void MultiTraceCamera::Update() {
    this->m_TargetCooridinate = this->GetTargetCooridinate();
}