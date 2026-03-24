#include "Scene/MapTest.hpp"

#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Component/IStateful.hpp"

namespace {

constexpr float kDoorMarginFromRoom = 70.0F;

} // namespace

MapTest::MapTest() : MapSystem() {
    this->m_MainRoom = std::make_shared<BaseRoom>(glm::vec2(0.0F, 0.0F));
    this->m_Pieces.push_back(this->m_MainRoom);

    this->m_MainPlayer = std::make_shared<Player>();
    this->m_MainPlayer->SetPosition({
        0.0F,
        -this->m_MainRoom->GetObjectSize().y / 2.0F - kDoorMarginFromRoom
    });
    this->m_Players.push_back(this->m_MainPlayer);

    this->m_AttachCamera = std::make_shared<TraceCamera>(
        this->m_MainPlayer,
        std::make_shared<EaseOutQubicCurve>()
    );

    this->m_AttachCamera->SetScale(glm::vec2(2.0F, 2.0F));

    if (this->m_MainPlayer != nullptr) {
        this->AddChild(this->m_MainPlayer);
    }

    for (const auto &piece : this->m_Pieces) {
        if (piece != nullptr) {
            this->AddChild(piece);
        }
    }
}

MapTest::~MapTest() = default;

void MapTest::Update() {
    this->Scene::Update();

    if (this->m_AttachCamera == nullptr) {
        return;
    }

    const std::shared_ptr<IStateful> statefulCamera =
        std::dynamic_pointer_cast<IStateful>(this->m_AttachCamera);
    if (statefulCamera != nullptr) {
        statefulCamera->Update();
    }

    if (this->m_MainPlayer != nullptr) {
        this->m_AttachCamera->SetTransformByCamera(this->m_MainPlayer);
    }

    for (const auto &piece : this->m_Pieces) {
        this->m_AttachCamera->SetTransformByCamera(piece);
    }
}
