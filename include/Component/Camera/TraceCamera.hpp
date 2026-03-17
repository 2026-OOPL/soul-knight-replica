#ifndef TRACE_CAMERA_HPP
#define TRACE_CAMERA_HPP

#include "Component/Camera/Camera.hpp"
#include "Component/IMapObject.hpp"
#include "Component/IStateful.hpp"
#include <glm/vec2.hpp>
#include <memory>

class TraceCamera : public Camera, IStateful {
public:
    TraceCamera(
        std::shared_ptr<IMapObject> target
    ) : Camera() {
        m_Target = target;
    }

    Util::Transform GetObjectTransform(std::shared_ptr<IMapObject> object) override;

    void Update() override;

protected:
    std::shared_ptr<IMapObject> m_Target;
};

#endif