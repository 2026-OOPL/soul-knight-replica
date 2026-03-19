set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp
    
    Component/Player/Player.cpp
    Component/TextButton/TextButton.cpp
    Component/Collision/CollisionSystem.cpp
    
    Component/Map/MapPiece.cpp   
    Component/Map/MapSystem.cpp
    
    Component/Camera/Camera.cpp
    Component/Camera/TraceCamera.cpp
    Component/Camera/MultiTraceCamere.cpp

    Scene/MapTest.cpp

    Component/Camera/Curve.cpp
)

set(INCLUDE_FILES
    App.hpp
    MainMenu.hpp
    Scene.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonPropertycpp.hpp
    Component/Character/Character.hpp
    Component/Collision/CollisionSystem.hpp

    Component/ICollidable.hpp
    Component/IStateful.hpp

    Scene/MapTest.hpp

    Component/Map/MapBase.hpp
    Component/Map/MapPiece.hpp

    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp

    CameraBase.hpp
    Config.hpp
)

set(TEST_FILES
)
