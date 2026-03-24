#include "Component/IMapObject.hpp"
#include "Core/Context.hpp"
#include "Util/GameObject.hpp"

#include <exception>
#include <glm/fwd.hpp>
#include <memory>
#include <stdexcept>

#include "Component/Camera/Camera.hpp"

glm::vec2 Camera::GetCameraSize() {
    auto context = Core::Context::GetInstance();
    return {context->GetWindowWidth(), context->GetWindowHeight()};
}

bool Camera::GetVisibilityByCamera(std::shared_ptr<IMapObject> object) {
    glm::vec2 objectSize = object->GetObjectSize();
    glm::vec2 cameraSize = this->GetCameraSize();

    glm::vec2 objectCooridinate = object->GetCooridinate();
    glm::vec2 cameraCooridinate = this->m_Transform.translation;

    glm::vec4 windowBorder = {
        cameraCooridinate.x - cameraSize.x / 2,
        cameraCooridinate.y + cameraSize.y / 2,
        cameraCooridinate.x + cameraSize.x / 2,
        cameraCooridinate.y - cameraSize.y / 2,
    };

    glm::vec4 mapBorder = {
        objectCooridinate.x - objectSize.x / 2,
        objectCooridinate.y + objectSize.y / 2,
        objectCooridinate.x + objectSize.x / 2,
        objectCooridinate.y - objectSize.y / 2,
    };

    const int margin = 10;

    const float mLeft = mapBorder.x - margin;
    const float mTop = mapBorder.y + margin;
    const float mRight = mapBorder.z + margin;
    const float mBottom = mapBorder.w - margin;

    const float wLeft = windowBorder.x;
    const float wTop = windowBorder.y;
    const float wRight = windowBorder.z;
    const float wBottom = windowBorder.w;

    const bool isOutside =
        (mRight < wLeft) ||
        (mLeft > wRight) ||
        (mBottom > wTop) ||
        (mTop < wBottom);

    return !isOutside;
}

void Camera::SetTransformByCamera(std::shared_ptr<Util::GameObject> object) {
    std::shared_ptr<IMapObject> mapObject = std::dynamic_pointer_cast<IMapObject>(object);

    if (mapObject == nullptr) {
        throw std::runtime_error("This object cannot be cast to IMapObject.");
    }

    const bool visible = this->GetVisibilityByCamera(mapObject);
    object->SetVisible(visible);

    if (visible) {
        object->m_Transform = this->GetTransformByCamera(mapObject);
    }
}

void Camera::SetCooridinate(glm::vec2 coordinate) {
    this->m_Transform.translation = coordinate;
}

void Camera::SetScale(glm::vec2 scale) {
    this->m_Transform.scale = scale;
}

void Camera::SetRotation(double degree) {
    this->m_Transform.rotation = degree;
}

glm::vec2 Camera::GetCooridinate() {
    return this->m_Transform.translation;
}

glm::vec2 Camera::GetScale() {
    return this->m_Transform.scale;
}

float Camera::GetRotation() {
    return this->m_Transform.rotation;
}
