#ifndef TRACE_CAMERA_HPP
#define TRACE_CAMERA_HPP

#include "Component/Camera/Camera.hpp"
#include "Component/Camera/Curve.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include <functional>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <memory>

class TraceCamera : public Camera, public IStateful {
public:
    TraceCamera(
        std::shared_ptr<IMapObject> target
    ) : Camera() {
        m_Target = target;
    }

    TraceCamera(
        std::shared_ptr<IMapObject> target,
        std::shared_ptr<Curve> curve
    ) : Camera() {
        m_Target = target;
        m_Curve = curve;
    }

    glm::vec2 GetTargetCooridinate() override;

    Util::Transform GetTransformByCamera(std::shared_ptr<IMapObject> object) override;

    void Update() override;

protected:
    glm::vec2 m_TargetCooridinate;
    std::shared_ptr<IMapObject> m_Target;
    std::shared_ptr<Curve> m_Curve;
};

#endif