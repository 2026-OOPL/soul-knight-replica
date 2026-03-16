set(SRC_FILES
    App.cpp
    MainMenu.cpp
    Scene.cpp

    Component/TextButton/TextButton.cpp
    Component/Map/MapPiece.cpp   

    Scene/MapTest.cpp


    Character.cpp
)

set(INCLUDE_FILES
    App.hpp
    MainMenu.hpp
    Scene.hpp

    Component/Player/Player.hpp
    Component/TextButton/TextButton.hpp
    Component/TextButton/TextButtonPropertycpp.hpp

    Component/ICollidable.hpp
    Component/IStateful.hpp

    Scene/MapTest.hpp

    Component/Map/MapPiece.hpp
    Component/Map/MapBase.hpp

    Character.hpp
)

set(TEST_FILES
)
