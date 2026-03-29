#ifndef TRACE_CAMERA_HPP
#define TRACE_CAMERA_HPP
#include <memory>

#include <glm/vec2.hpp>

#include "Common/MapObject.hpp"
#include "Component/Camera/Camera.hpp"
#include "Component/Camera/Curve.hpp"
#include "Component/IStateful.hpp"

class TraceCamera : public Camera, public IStateful {
public:
    TraceCamera(
        std::shared_ptr<MapObject> target
    ) : Camera() {
        m_Target = target;
    }

    TraceCamera(
        std::shared_ptr<MapObject> target,
        std::shared_ptr<Curve> curve
    ) : Camera() {
        m_Target = target;
        m_Curve = curve;
    }

    glm::vec2 GetTargetCooridinate() override;

    Util::Transform GetTransformByCamera(std::shared_ptr<MapObject> object) override;

    void Update() override;

protected:
    glm::vec2 m_TargetCooridinate;
    std::shared_ptr<MapObject> m_Target;
    std::shared_ptr<Curve> m_Curve;
};

#endif