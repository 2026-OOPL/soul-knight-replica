set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp

    Component/TextButton/TextButton.cpp
    Component/Map/MapPiece.cpp   

    Scene/MapTest.cpp

    Component/Camera/TraceCamera.cpp
    Component/Player/Player.cpp
    Component/Map/MapSystem.cpp
    Component/Camera/Camera.cpp
)

set(INCLUDE_FILES
    App.hpp
    MainMenu.hpp
    Scene.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonPropertycpp.hpp
    Component/Character/Character.hpp

    Component/ICollidable.hpp
    Component/IStateful.hpp

    Scene/MapTest.hpp

    Component/Map/MapPiece.hpp
    Component/Map/MapBase.hpp

    CameraBase.hpp
    Config.hpp
)

set(TEST_FILES
)
