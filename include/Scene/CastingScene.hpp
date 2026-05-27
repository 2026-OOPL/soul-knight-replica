#ifndef CASTING_SCENE_HPP
#define CASTING_SCENE_HPP

#include <memory>

#include "Component/Camera/Camera.hpp"
#include "Scene.hpp"
#include "Util/Time.hpp"

class CastingScene : public Scene {
public:
    CastingScene();

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;
    
private:
    std::shared_ptr<Camera> m_AttachedCamera;
    std::shared_ptr<Scene> m_SceneRedirection = nullptr;

    Util::ms_t m_CastingEndTime = -1.0F;

    double m_Progress = 0.0F;
};

#endif