#ifndef MAP_TEST_HPP
#define MAP_TEST_HPP

#include <memory>
#include <vector>

#include "Component/Map/BaseRoom.hpp"
#include "Component/Map/Gangway.hpp"
#include "Component/Map/MapSystem.hpp"
#include "Component/UI/PauseUI.hpp"
#include "Component/UI/PlayUI.hpp"
#include "Generator/MapGenerator.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"

class MapTest : public MapSystem {
public:
    MapTest();
    
    MapTest(
        MapSystemConfig::MapConfig config
    );

    ~MapTest() override = default;

    void Update() override;

    std::shared_ptr<Scene> GetRedirection() override;

private:
    std::vector<std::shared_ptr<BaseRoom>> m_RoomsInScene;
    std::vector<std::shared_ptr<Gangway>> m_GangwaysInScene;
    std::shared_ptr<BaseRoom> m_MainRoom;
    std::shared_ptr<Player> m_MainPlayer;
    
    std::string m_LevelName;
    
    std::shared_ptr<PlayUI> m_PlayUI;
    
    std::shared_ptr<Util::GameObject> m_LevelIcon;
    std::shared_ptr<Util::GameObject> m_LevelTitle;
    Util::ms_t m_SceneStartTime;
    
    bool m_IsPaused = false;
    bool m_ExitToHome = false;
    std::shared_ptr<PauseUI> m_PauseUI;
};

#endif
