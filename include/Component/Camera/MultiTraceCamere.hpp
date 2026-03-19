#ifndef MULTI_TRACE_CAMERA_HPP
#define MULTI_TRACE_CAMERA_HPP

#include "Component/Camera/Camera.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

class MultiTraceCamera : public Camera, IStateful {
public:
    MultiTraceCamera(
        std::vector<std::shared_ptr<IMapObject>>& target
    ) : Camera() {
        m_Target = target;
    }

    glm::vec2 GetTargetCooridinate() override;
    Util::Transform GetTransformByCamera(std::shared_ptr<IMapObject> object) override;

    void Update() override;

protected:
    // Store multiple target that needs to be tracked
    std::vector<std::shared_ptr<IMapObject>> m_Target;
    
    // This stores the final cooridinate the camera needs to go
    // But the smooth camera function might reduce the motivation of the trace
    // So the camera position is not always follows this cooridinate 
    glm::vec2 m_TargetCooridinate;
};

#endif