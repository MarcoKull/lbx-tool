cmake_minimum_required(VERSION 2.6)
project(lbx-tool)

# compiler options
if (NOT MSVC)
    add_definitions(-std=c++11 -pipe -pedantic -Wall -Wconversion -Werror)
endif()

# search qt
if (WITH_QT OR NOT DEFINED WITH_QT)
    find_package(Qt5 COMPONENTS Core Widgets)
    if(Qt5_FOUND)
        add_definitions(-DWITH_QT)
        set(CMAKE_AUTOMOC TRUE)
        set(CMAKE_AUTORCC TRUE)

        # set gui source files and link targets
        set(GUI_LINK Qt5::Core Qt5::Widgets)
        set(GUI_SOURCES
            resources/lbx-tool.qrc
            src/gui/Command.cpp
            src/gui/Model.cpp
            src/gui/Window.cpp
        )

        # install mime type, icon and desktop file on linux
        if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            install(FILES resources/lbx-tool.desktop DESTINATION /usr/share/applications)
            install(FILES resources/lbx-tool.png DESTINATION /usr/share/pixmaps)
            install(FILES resources/lbx-mime.xml DESTINATION /usr/share/mime/packages)
        endif()

    elseif(WITH_QT)
        message(FATAL_ERROR "qt5 library not found")
    endif()
endif()

# lbx-tool
add_executable(lbx-tool src/tool.cpp ${GUI_SOURCES})
install(
    TARGETS lbx-tool
    RUNTIME DESTINATION bin
)
target_link_libraries(lbx-tool lbx ${GUI_LINK})
