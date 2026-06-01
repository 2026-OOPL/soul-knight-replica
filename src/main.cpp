#include "App.hpp"
#include "Component/UI/Advertisement.hpp"
#include "Core/Context.hpp"

int main(int, char**) {
    App app;
    auto context = Core::Context::GetInstance();
    
    context->SetWindowIcon(RESOURCE_DIR"/icon.png");

    // Advertisement preload
    std::shared_ptr<AdvertisementUI> advertisementUI = std::make_shared<AdvertisementUI>(
        AdvertisementArc::SPRITES,
        AdvertisementArc::SPRITE_NUMBER
    );

    advertisementUI->PreloadMedia();

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case App::State::START:
                app.Start();
                break;

            case App::State::UPDATE:
                app.Update();
                break;

            case App::State::END:
                app.End();
                context->SetExit(true);
                break;
        }
        
        context->Update();
    }
    
    return 0;
}
