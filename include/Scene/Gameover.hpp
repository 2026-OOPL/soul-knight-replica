#ifndef GAMEOVER_SCENE_HPP
#define GAMEOVER_SCENE_HPP

#include <memory>

#include "Component/Player/Knight.hpp"
#include "Scene.hpp"
#include "Util/BGM.hpp"
#include "Component/Button/ImageButton.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"

class GameoverScene : public Scene {
public:
    GameoverScene(MapSystemConfig::MapConfig config);

    ~GameoverScene() = default;

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;
    
private:
    std::shared_ptr<Knight> m_Knight;
    std::shared_ptr<ImageButton> m_ContinueButton;

    std::shared_ptr<Util::Text> m_LevelText;
    std::shared_ptr<Util::GameObject> m_LevelTextObject;
    std::shared_ptr<Util::GameObject> m_Background;

    std::shared_ptr<Scene> m_SceneRedirection = nullptr;

    std::shared_ptr<Util::BGM> m_BGM;

    bool m_SwitchBGM = false;

    Util::ms_t m_SceneStartTime = 0;

    void MoveCharacterAnimation();

    float m_AnimationEndPositionX;

    MapSystemConfig::MapConfig m_Config;

    std::shared_ptr<Util::GameObject> m_Ghostking;
    std::shared_ptr<Util::GameObject> m_VitaminCMecha;
    std::shared_ptr<Util::GameObject> m_ZulanInRuins;
    std::shared_ptr<Util::GameObject> m_TextGhostking;
    std::shared_ptr<Util::GameObject> m_TextVitaminCMecha;
    std::shared_ptr<Util::GameObject> m_TextZulanInRuins;

    float GetAnimationEndPositionX(MapSystemConfig::MapInfo info) const;
    std::string GetLevelTextByPosition(float positionX) const;
};

#endif