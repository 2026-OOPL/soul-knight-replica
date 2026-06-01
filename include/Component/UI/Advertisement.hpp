#ifndef ADVERTISEMENT_UI_HPP
#define ADVERTISEMENT_UI_HPP

#include <memory>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "Component/Button/TextButton.hpp"
#include "Component/UI/BaseUI.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

namespace AdvertisementArc {
    const std::vector<std::string> SPRITES = {
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_0.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_1.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_2.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_3.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_4.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_5.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_6.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_7.png",
        RESOURCE_DIR"/Advertisement/ArcBrowser/spritesheet_8.png"
    };

    constexpr int SPRITE_NUMBER = 878;
}

class AdvertisementUI : public BaseUI {

public:
    AdvertisementUI(
        std::vector<std::string> videoFrames,
        int spriteNumber
    );

    ~AdvertisementUI() override = default;

    void Update() override;

    void PreloadMedia();

private:
    std::shared_ptr<TextButton> m_LeaveButton;
    std::shared_ptr<Util::Animation> m_ViewFrame;
    std::shared_ptr<Util::Animation> m_Animation;
    std::shared_ptr<Util::GameObject> m_Video;

    Util::ms_t m_AnimationStartTime;
    
    bool m_IsBGMStarted = false;
    bool m_IsButtonPresented = false;

    std::shared_ptr<Util::BGM> m_BGM;

    int m_CurrentFrame = 1;

    std::vector<std::string> m_VideoFrames;
    int m_SpriteNumber;

    void SetSprite(int frame);
};

#endif 