set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp

    Scene/MapTest.cpp

    Common/Random.cpp
    
    Component/Camera/Curve.cpp
    Component/Character/Character.cpp
    Component/Collision/CollisionSystem.cpp
    Component/Debug/CollisionDebugOverlay.cpp
    Component/Debug/CollisionDebugSnapshot.cpp
    Component/Debug/SolidColorRectDrawable.cpp
    Component/Player/Player.cpp
    Component/Player/Knight.cpp
    Component/TextButton/TextButton.cpp
    Component/UI/PlayUI.cpp

    Component/Map/BaseRoom.cpp
    Component/Map/FightRoom.cpp
    Component/Map/Gangway.cpp
    Component/Map/MapPiece.cpp
    Component/Map/RectMapArea.cpp
    Component/Map/Door.cpp
    Component/Map/MapSystem.cpp
    Component/Map/StarterRoom.cpp
    
    Component/Camera/Camera.cpp
    Component/Camera/TraceCamera.cpp
    Component/Camera/MultiTraceCamere.cpp

    Generator/GenChamber.cpp
    Generator/GenFightChamber.cpp
    Generator/GenRewardChamber.cpp
    Generator/MapBlueprint.cpp
    Generator/MapGenerator.cpp
    Generator/GenPortalChamber.cpp

)

set(INCLUDE_FILES
    App.hpp
    MainMenu.hpp
    Scene.hpp

    Component/Collision/CollisionSystem.hpp
    Common/CombatFaction.hpp
    Common/Random.hpp
    Common/Enums.hpp
    Common/EnumMask.hpp
    Common/Constants.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonProperty.hpp
    Component/Character/Character.hpp
    Component/Debug/CollisionDebugOverlay.hpp
    Component/Debug/CollisionDebugSnapshot.hpp
    Component/Debug/SolidColorRectDrawable.hpp

    Component/IMapObject.hpp
    Component/IStateful.hpp

    Scene/MapTest.hpp

    Component/Map/BaseRoom.hpp
    Component/Map/Door.hpp
    Component/Map/FightRoom.hpp
    Component/Map/Gangway.hpp
    Component/Map/GangwayLayoutConfig.hpp
    Component/Map/MapColliderConfig.hpp
    Component/Map/MapPiece.hpp
    Component/Map/MapSystem.hpp
    Component/Map/RectMapArea.hpp
    Component/Map/StarterRoom.hpp

    Component/Camera/Camera.hpp
    Component/Camera/TraceCamera.hpp
    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp
    Component/IStateful.hpp
    Component/Map/MapPiece.hpp
    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp
    Component/Player/Knight.hpp
    Component/Player/Player.hpp
    Component/UI/PlayUI.hpp


    Generator/GenChamber.hpp
    Generator/GenPortalChamber.hpp
    Generator/GenFightChamber.hpp
    Generator/GenRewardChamber.hpp
    Generator/MapBlueprint.hpp
    Generator/MapGenerator.hpp
)

set(TEST_FILES
)
