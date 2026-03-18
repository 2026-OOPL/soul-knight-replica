#include "Core/Context.hpp"
#include <glm/fwd.hpp>
#include "Component/Camera/Camera.hpp"

glm::vec2 Camera::GetCameraSize() {
    auto context = Core::Context::GetInstance();
    return {context->GetWindowWidth(), context->GetWindowHeight()};
}

bool Camera::GetVisibility(std::shared_ptr<IMapObject> object) {
    glm::vec2 objectSize = object->GetObjectSize();
    glm::vec2 cameraSize = this->GetCameraSize();

    glm::vec2 objectCooridinate = object->GetCooridinate();
    glm::vec2 cameraCooridinate = this->m_Transform.translation;

    // Current window view port border in LTRB
    glm::vec4 windowBorder = {
        cameraCooridinate.x - cameraSize.x / 2,
        cameraCooridinate.y + cameraSize.y / 2,
        cameraCooridinate.x + cameraSize.x / 2,
        cameraCooridinate.y - cameraSize.y / 2,
    };

    // Current MapPiece render border in LTRB
    glm::vec4 mapBorder = {
        objectCooridinate.x - objectSize.x / 2,
        objectCooridinate.y + objectSize.y / 2,
        objectCooridinate.x + objectSize.x / 2,
        objectCooridinate.y - objectSize.y / 2,
    };

    const int margin = 10;
    
    float mLeft   = mapBorder.x - margin;
    float mTop    = mapBorder.y + margin;
    float mRight  = mapBorder.z + margin;
    float mBottom = mapBorder.w - margin;

    float wLeft   = windowBorder.x;
    float wTop    = windowBorder.y;
    float wRight  = windowBorder.z;
    float wBottom = windowBorder.w;

    bool isOutside = 
        (mRight  < wLeft  ) || // 整張在地圖左邊
        (mLeft   > wRight ) || // 整張在地圖右邊
        (mBottom > wTop   ) || // 整張在地圖上方 (Bottom 比 Window Top 還高)
        (mTop    < wBottom);   // 整張在地圖下方 (Top 比 Window Bottom 還低)

    return !isOutside;
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

double Camera::GetRotation() {
    return this->m_Transform.rotation;
}