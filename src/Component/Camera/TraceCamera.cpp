#include "Util/Transform.hpp"
#include <Component/Camera/TraceCamera.hpp>
#include <Component/IMapObject.hpp>
#include <glm/fwd.hpp>

Util::Transform TraceCamera::GetObjectTransform(std::shared_ptr<IMapObject> object) {
    // The view center of this camera 
    glm::vec2 cameraCooridinate = m_Target->GetCooridinate();
    glm::vec2 objectCooridinate = object->GetCooridinate();

    return {
        objectCooridinate - cameraCooridinate,
        0,
        {1, 1 }
    };
}

void TraceCamera::Update() {
    this->m_Transform.translation = m_Target->GetCooridinate();
}