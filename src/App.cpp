#include <memory>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Renderer.hpp"

#include "App.hpp"
#include "MainMenu.hpp"

void App::Start() {
    LOG_TRACE("Start");

    this->m_Scene = std::make_shared<MainMenu>();
    this->m_Root.AddChild(this->m_Scene);

    this->m_CurrentState = State::UPDATE;
}

void App::Update() {
    const std::shared_ptr<Scene> newScene = this->m_Scene->GetRedirection();

    if (newScene) {
        this->m_Root.RemoveChild(this->m_Scene);
        this->m_Scene = newScene;
        this->m_Root.AddChild(this->m_Scene);
    }

    this->m_Scene->Update();
    
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        this->m_CurrentState = State::END;
    }

    this->m_Root.Update();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    if (this->m_Scene != nullptr) {
        this->m_Root.RemoveChild(this->m_Scene);
        this->m_Scene = nullptr;
    }

    LOG_TRACE("End");
}
