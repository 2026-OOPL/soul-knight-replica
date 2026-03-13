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

    m_Scene = std::make_unique<MainMenu>(&m_Root);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
     
    std::unique_ptr<Scene> newScene = m_Scene->GetRedirection();

    if (newScene) {
        m_Scene = std::move(newScene);
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
