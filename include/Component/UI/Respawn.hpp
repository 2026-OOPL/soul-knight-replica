#ifndef RESPAWN_UI_HPP
#define RESPAWN_UI_HPP

#include <memory>

#include <glm/vec2.hpp>

#include "Component/Button/ImageButton.hpp"
#include "Component/UI/Advertisement.hpp"
#include "Component/UI/BaseUI.hpp"
#include "Util/GameObject.hpp"

class RespawnUI : public BaseUI {
public:
    RespawnUI(
        std::function<void()> onLeaveButtonClick,
        std::function<void()> onVideoButtonClick
    );

    ~RespawnUI() override = default;

    void Update() override;

    bool GetExitSignal();

    void ToggleSettings();

private:
    bool m_ExitSignal = false;

    std::shared_ptr<Util::GameObject> m_RespawnMenu;
    std::shared_ptr<Util::GameObject> m_Background;

    std::shared_ptr<ImageButton> m_LeaveButton;
    std::shared_ptr<ImageButton> m_VideoButton;

    bool isInsideHomeButton();
    bool isInsideContinueButton();
    bool isInsideSettingsButton();

    std::function<void()> onLeaveButtonClick;
    std::function<void()> onVideoButtonClick;

    std::shared_ptr<AdvertisementUI> m_AdvertisementUI;

    void SetAdvertisement(bool open);
};

#endif 