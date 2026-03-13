
#include "Scene/MapTest.hpp"
#include "Component/TextButton/TextButton.hpp"

#include "Util/Input.hpp"

void MapTest::Initialize() {
    m_Root->AddChild(
        std::make_shared<TextButton>(
        "載入存檔", 2,
        nullptr
    ));

    for (auto& i : m_Pieces) {
        m_Root->AddChild(i);
    }
}

void MapTest::Dispose() {
    return;
}

void MapTest::Update() {
    for (std::shared_ptr<MapPiece> i : this->pieces) {
        bool visible = i->GetVisibilityByCooridinate(m_Cooridinate);

        i->SetVisible(visible);

        if (visible) {
            i->SetTransformByCooridinate(m_Cooridinate);
        }
    }

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
}