#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <memory>
#include <utility>
#include "MainMenu.hpp"
#include "Util/Renderer.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_Scene = std::make_shared<MainMenu>();
    m_Scene->Initialize();

    m_CurrentState = State::UPDATE;
}

void App::Update() {
     
    std::shared_ptr<Scene> newScene = m_Scene->GetRedirection();

    if (newScene) {
        this->m_Root.RemoveChild(m_Scene);
        m_Scene->Dispose();

        m_Scene = newScene;
        m_Scene->Initialize();

        this->m_Root.AddChild(m_Scene);
    }

    m_Scene->Update();
    
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
