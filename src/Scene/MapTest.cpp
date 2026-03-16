
#include "Scene/MapTest.hpp"
#include "Component/Map/MapPiece.hpp"
#include "Component/TextButton/TextButton.hpp"

#include "Component/Map/BaseRoom.hpp"
#include "Scene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include <memory>

void MapTest::Initialize() {
    this->AddChild(std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR"/Character/Character.png"
        ),
        1
    ));

    LOG_INFO("Game started !");

    this->AddChild(std::make_shared<BaseRoom>(
        glm::vec2(0, 0)
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(-1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(1280, 0),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));
    
    // asdsadsad
    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(0, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(-1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(1280, 720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    // sadasds
    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(0, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(-1280, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));

    this->AddChild(std::make_shared<MapPiece>(
        glm::vec2(1280, -720),
        RESOURCE_DIR"/Map/StageTest/Map_0_0.png"
    ));
    
    for (auto& i : m_Pieces) {
        this->AddChild(i);
    }
}

void MapTest::Dispose() {
    return;
}

void MapTest::Update() {
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) {
        m_Cooridinate.y -= 10;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
        m_Cooridinate.y += 10;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        m_Cooridinate.x += 10;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        m_Cooridinate.x -= 10;
    }

    for (auto& i : this->m_Children) {
        i->m_Transform.scale = this->m_Transform.scale;

        std::shared_ptr<MapPiece> pieces = std::dynamic_pointer_cast<MapPiece>(i);

        if (pieces) {
            bool visible = pieces->GetVisibilityByCooridinate(m_Cooridinate);
        
            pieces->SetVisible(visible);

            if (visible) {
                pieces->SetTransformByCooridinate(m_Cooridinate);
            }

            continue;
        }
    }

    Scene::Update();
}