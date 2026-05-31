#ifndef LEVEL_CHOOSE_HPP
#define LEVEL_CHOOSE_HPP

#include <imgui.h>
#include <memory>

#include "Scene/MapTest.hpp"
#include "Util/GameObject.hpp"

#include "Scene.hpp"
#include "Util/Time.hpp"

class LevelSwitch : public Scene {
public:
    LevelSwitch(MapSystemConfig::MapConfig config);

    virtual ~LevelSwitch() = default;

    std::shared_ptr<Scene> GetRedirection() override;

    void Update() override;

private:
    void SwitcToNextLevel(); 

    Util::ms_t m_SceneStartTime;
    std::shared_ptr<Util::GameObject> m_LoadingText;
    std::shared_ptr<Scene> m_Redirect_Scene;

    MapSystemConfig::MapConfig m_MapConfig;

    std::shared_ptr<ImageButton> m_LeftSkill;
    std::shared_ptr<ImageButton> m_MiddleSkill;
    std::shared_ptr<ImageButton> m_RightSkill;
};

#endif
