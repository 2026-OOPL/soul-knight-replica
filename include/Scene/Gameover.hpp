#ifndef GAMEOVER_SCENE_HPP
#define GAMEOVER_SCENE_HPP

#include <memory>
#include <vector>

#include "Component/Button/ImageButton.hpp"
#include "Component/Camera/Camera.hpp"
#include "Scene.hpp"
#include "Util/BGM.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

class GameoverScene : public Scene {
public:
    GameoverScene();

    ~GameoverScene() = default;

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;
    
private:
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<ImageButton> m_ContinueButton;

    std::shared_ptr<Scene> m_SceneRedirection = nullptr;

    std::shared_ptr<Util::BGM> m_BGM;

    bool m_SwitchBGM = false;
};

#endif