#ifndef PRELOAD_SCENE_HPP
#define PRELOAD_SCENE_HPP

#include <memory>

#include "Scene.hpp"
#include "Util/BGM.hpp"
#include "Component/Player/Knight.hpp"
#include "Component/Button/ImageButton.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

class PreloadScene : public Scene {
public:
    PreloadScene();

    ~PreloadScene() = default;

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;

    void StartPreload();
    
private:
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_Text;

    std::shared_ptr<Scene> m_RedirectScene;

    std::shared_ptr<Util::BGM> m_BGM;

    Util::ms_t m_SceneStartTime;

    bool m_BGMStarted = false;
    
};

#endif