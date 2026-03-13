#include "Scene/MapTest.hpp"
#include "Component/TextButton/TextButton.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include <memory>

void MapTest::Initialize() {
    m_Root->AddChild(
        std::make_shared<TextButton>(
        "載入存檔", 2,
        nullptr
    ));
}

void MapTest::Dispose() {
    return;
}

void MapTest::Update() {
    
}