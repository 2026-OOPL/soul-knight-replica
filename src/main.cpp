#include <memory>

#include "App.hpp"
#include "Core/Context.hpp"
#include "Generator/MapGenerator.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    App app;    

    std::shared_ptr<MapGenerator> generator = std::make_shared<MapGenerator>(
        "testaaa"
    );

    generator->Generate();

    generator->m_Blueprint->OutputMapGridType();
    
    context->SetWindowIcon(RESOURCE_DIR"/icon.png");

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
