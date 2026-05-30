#ifndef CASTING_SCENE_HPP
#define CASTING_SCENE_HPP

#include <memory>

#include "Component/Camera/Camera.hpp"
#include "Scene.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Time.hpp"

class CastingScene : public Scene {
public:
    CastingScene();

    ~CastingScene();

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;
    
private:
    std::shared_ptr<Camera> m_AttachedCamera;
    std::shared_ptr<Scene> m_SceneRedirection = nullptr;

    std::shared_ptr<Util::BGM> m_Music;

    Util::ms_t m_SceneStartTime;
    Util::ms_t m_CastingEndTime = -1.0F;

    float m_Progress = 0.0F;
    float m_ScrollingSpeed = 1.0F;

    bool m_SwitchBGM = false;
};

#endif