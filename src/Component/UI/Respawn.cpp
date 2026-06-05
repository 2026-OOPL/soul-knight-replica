#include <memory>

#include "Component/UI/Respawn.hpp"
#include "Component/Button/ImageButton.hpp"

RespawnUI::RespawnUI(
    std::function<void()> onLeaveButtonClick,
    std::function<void()> onVideoButtonClick
) : BaseUI(true) {
    const int baseZIndex = this->GetZIndex();

    this->onLeaveButtonClick = onLeaveButtonClick;
    this->onVideoButtonClick = onVideoButtonClick;

    m_RespawnMenu = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/UI/Respawn/respawn_menu.png"
        ),baseZIndex + 1
    );

    this->AddChild(m_RespawnMenu);

    m_LeaveButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() {
                
                this->onLeaveButtonClick();
                this->m_ExitSignal = true;
            }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR "/UI/Respawn/respawn_button_leave.png",
            RESOURCE_DIR "/UI/Respawn/respawn_button_leave.png",
            RESOURCE_DIR "/UI/Respawn/respawn_button_leave.png"
        ),
        nullptr
    );
    
    m_LeaveButton->SetZIndex(baseZIndex + 2);
    m_LeaveButton->m_Transform.translation = {-152.0F, -48.0F};
    this->AddChild(m_LeaveButton);

    m_VideoButton = std::make_shared<ImageButton>(
        std::make_shared<ButtonAction>(
            nullptr,
            nullptr,
            [this]() { this->SetAdvertisement(true); }
        ),
        std::make_shared<ImageButtonTheme>(
            RESOURCE_DIR "/UI/Respawn/respawn_button_video.png",
            RESOURCE_DIR "/UI/Respawn/respawn_button_video.png",
            RESOURCE_DIR "/UI/Respawn/respawn_button_video.png"
        ),
        nullptr
    );

    m_VideoButton->SetZIndex(baseZIndex + 2);
    m_VideoButton->m_Transform.translation = {60.0F, -48.0F};
    this->AddChild(m_VideoButton);

}

void RespawnUI::Update() {
    if (m_AdvertisementUI && m_AdvertisementUI->GetExitSignal()) {
        this->m_ExitSignal = true;
        this->SetAdvertisement(false);
        this->onVideoButtonClick();
        return;
    }

    if (m_AdvertisementUI) {
        m_AdvertisementUI->Update();
    } else {
        BaseUI::Update();
    }
}

bool RespawnUI::GetExitSignal() {
    return this->m_ExitSignal;
}

void RespawnUI::SetAdvertisement(bool open) {
    if (open) {
        m_AdvertisementUI = std::make_shared<AdvertisementUI>(
            AdvertisementArc::SPRITES,
            AdvertisementArc::SPRITE_NUMBER
        );

        m_AdvertisementUI->SetZIndex(this->GetZIndex() + 10);
        this->AddChild(m_AdvertisementUI);
    } else {
        if (!m_AdvertisementUI) {
            return;
        }

        this->RemoveChild(m_AdvertisementUI);
        m_AdvertisementUI = nullptr;
    }
}