set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp

    Component/Collision/CollisionSystem.cpp
    Component/Player/Player.cpp
    Component/TextButton/TextButton.cpp

    Component/Map/BaseRoom.cpp
    Component/Map/MapPiece.cpp
    Component/Map/Door.cpp
    Component/Map/MapSystem.cpp
    Component/Map/RoomAssembly.cpp
    
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

    Component/Collision/CollisionSystem.hpp
    Common/Random.hpp
    Common/Enums.hpp
    Common/EnumMask.hpp
    Common/Constants.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonProperty.hpp
    Component/Character/Character.hpp

    Component/IMapObject.hpp
    Component/IStateful.hpp

    Scene/MapTest.hpp

    Component/Map/BaseRoom.hpp
    Component/Map/Door.hpp
    Component/Map/MapPiece.hpp
    Component/Map/MapSystem.hpp
    Component/Map/RoomAssembly.hpp

    Component/Camera/Camera.hpp
    Component/Camera/TraceCamera.hpp
    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp
    Component/IStateful.hpp
    Component/Map/MapPiece.hpp
    Component/Camera/MultiTraceCamere.hpp
    Component/Camera/Curve.hpp
    
    Generator/MapGenerator.hpp
    Generator/JointRoomGen.hpp

    Scene/MapTest.hpp
)

set(TEST_FILES
)
