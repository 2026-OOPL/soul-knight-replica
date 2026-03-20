#include <glm/fwd.hpp>
#include <memory>

#include "Component/Camera/Curve.hpp"
#include "Component/IStateful.hpp"
#include "Component/IMapObject.hpp"
#include "Component/Map/BaseRoom.hpp"
#include "Component/Camera/TraceCamera.hpp"
#include "Scene.hpp"
#include "Scene/MapTest.hpp"
#include "Util/Time.hpp"

MapTest::MapTest() : MapSystem() {
    m_MainPlayer = std::make_shared<Player>();
    m_MainPlayer->SetPosition({0.0F, 0.0F});
    m_Players.push_back(m_MainPlayer);
    
    m_AttachCamera = std::make_shared<TraceCamera>(
        m_MainPlayer, std::make_shared<EaseOutQubicCurve>()
    );

    m_AttachCamera->SetScale(glm::vec2(2 ,2));

    m_Pieces.push_back(std::make_shared<BaseRoom>(
        glm::vec2(0, 0)
    ));

    this->AddChild(m_MainPlayer);

    for (auto const& i : m_Pieces) {
        this->AddChild(i);
    }
}

MapTest::~MapTest() {
    this->RemoveChild(m_MainPlayer);

    for (auto const& i : m_Pieces) {
        this->RemoveChild(i);
    }
}

void MapTest::Update() {    
    for (const auto &piece : m_Pieces) {
        m_AttachCamera->SetTransformByCamera(piece);
    }

    m_MainPlayer->Update();

    // Update camera if available
    std::shared_ptr<IStateful> statefulCamera = std::dynamic_pointer_cast<IStateful>(m_AttachCamera);
    if (statefulCamera) {
        statefulCamera->Update();
    }

    Scene::Update();
}