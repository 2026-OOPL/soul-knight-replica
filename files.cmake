set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp
    
    Component/Player/Player.cpp
    Component/TextButton/TextButton.cpp
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

    Common/Random.hpp
    Common/Enums.hpp
    Common/EnumMask.hpp
    Common/Constants.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonPropertycpp.hpp
    Component/Character/Character.hpp
    Component/ICollidable.hpp
    Component/IStateful.hpp
    Component/Map/MapBase.hpp
    Component/Map/MapPiece.hpp
    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp
    
    Generator/MapGenerator.hpp
    Generator/JointRoomGen.hpp

    Scene/MapTest.hpp
)

set(TEST_FILES
)
