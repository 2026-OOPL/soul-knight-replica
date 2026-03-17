#ifndef TRACE_CAMERA_HPP
#define TRACE_CAMERA_HPP

#include "Component/Camera/Camera.hpp"
#include "Component/IMapObject.hpp"
#include <glm/vec2.hpp>
#include <memory>

class TraceCamera : public Camera {
public:
    TraceCamera(
        std::shared_ptr<IMapObject> target,
        Util::Transform transform
    ) : Camera(transform) {
        m_Target = target;
    }

    Util::Transform GetObjectTransform(std::shared_ptr<IMapObject> object) override;

protected:
    std::shared_ptr<IMapObject> m_Target;
};

#endif