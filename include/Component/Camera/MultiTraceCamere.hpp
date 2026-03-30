#ifndef MULTI_TRACE_CAMERA_HPP
#define MULTI_TRACE_CAMERA_HPP

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/IStateful.hpp"


class MultiTraceCamera : public Camera, IStateful {
public:
    MultiTraceCamera(
        std::vector<std::shared_ptr<MapObject>>& target
    ) : Camera() {
        m_Target = target;
    }

    glm::vec2 GetTargetCooridinate() override;
    Util::Transform GetTransformByCamera(std::shared_ptr<MapObject> object) override;

    void Update() override;

protected:
    // Store multiple target that needs to be tracked
    std::vector<std::shared_ptr<MapObject>> m_Target;
    
    // This stores the final cooridinate the camera needs to go
    // But the smooth camera function might reduce the motivation of the trace
    // So the camera position is not always follows this cooridinate 
    glm::vec2 m_TargetCooridinate;
};

#endif