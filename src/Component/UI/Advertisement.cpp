#include <memory>
#include <string>

#include "Component/UI/BaseUI.hpp"
#include "Component/UI/Advertisement.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"

AdvertisementUI::AdvertisementUI(
    std::vector<std::string> videoFrames,
    int spriteNumber
) : BaseUI(false) {

    this->m_VideoFrames = videoFrames;
    this->m_SpriteNumber = spriteNumber;

    const float baseZIndex = this->GetZIndex();

    m_ViewFrame = std::make_shared<Util::Animation>(
        videoFrames,
        false,
        1.0F/24*100*1000 // 24 fps, 100 frames per sprite, 1000 ms per second
    );

    m_Video = std::make_shared<Util::GameObject>(
        m_ViewFrame, baseZIndex + 1
    );

    m_Video->m_Transform.scale = {2.0F, 2.0F};

    this->AddChild(m_Video);

    m_AnimationStartTime = Util::Time::GetElapsedTimeMs();

    this->m_BGM = std::make_shared<Util::BGM>(
        RESOURCE_DIR "/SFX/Arc on Windows. Download now.mp3"
    );

    this->m_LeaveButton = std::make_shared<TextButton>(
        "X",
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->m_ExitSignal = true; }
        ),
        std::make_shared<TextButtonTheme>(
            Util::Color(255, 255, 255),
            Util::Color(255, 255, 255),
            Util::Color(255, 255, 255)
        )
    );
}

void AdvertisementUI::Update() {
    BaseUI::Update();
    
    if (!m_IsBGMStarted) {
        m_IsBGMStarted = true;
        m_BGM->Play(0);
    }

    if (m_CurrentFrame >= m_SpriteNumber - 5 && !m_IsButtonPresented) {
        m_IsButtonPresented = true;
        this->m_LeaveButton->m_Transform.translation = {580.0F, 300.0F};
        this->m_LeaveButton->SetZIndex(this->GetZIndex() + 2);
        this->AddChild(this->m_LeaveButton);
    }
    
    if (m_CurrentFrame + 1 < m_SpriteNumber) {
        m_CurrentFrame = 1 + (Util::Time::GetElapsedTimeMs() - m_AnimationStartTime) / (1.0F / 24 * 1000);

        this->SetSprite(m_CurrentFrame);
    }
}

void AdvertisementUI::SetSprite(int frame) {
    m_ViewFrame->SetCurrentFrame(frame / 100);
  
    int row = (frame%100) / 10;
    int col = (frame%100) % 10;

    m_Video->m_Transform.translation = {
        ( (6400/2) - 320 - col * 640 ) * m_Video->m_Transform.scale.x,
        ( -(3600/2) + 180 + row * 360 ) * m_Video->m_Transform.scale.y
    };
}

void AdvertisementUI::PreloadMedia() {
    std::make_shared<Util::Animation>(
        m_VideoFrames,
        24*100*1000, // 24 fps, 100 frames per sprite, 1000 ms per second
        true
    );
}