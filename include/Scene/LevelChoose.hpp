#ifndef LEVEL_CHOOSE_HPP
#define LEVEL_CHOOSE_HPP

#include <imgui.h>
#include <memory>

#include "Util/GameObject.hpp"

#include "Scene.hpp"
#include "Util/Time.hpp"

class LevelSwitch : public Scene {
public:
    LevelSwitch();

    virtual ~LevelSwitch() = default;

    std::shared_ptr<Scene> GetRedirection() override;

    void Update() override;

private:
    void StartNewGame();
    void LeaveGame();

    Util::ms_t m_SceneStartTime;
    std::shared_ptr<Util::GameObject> m_LoadingText;
    std::shared_ptr<Scene> m_Redirect_Scene;
};

#endif
