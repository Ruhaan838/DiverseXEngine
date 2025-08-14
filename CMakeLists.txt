cmake_minimum_required(VERSION 3.16)

project(DiverseXEngine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        main.ui
        core/nodes/node.cpp
        core/nodes/node.h
        core/nodes/edge.cpp
        core/nodes/edge.h
        core/nodes/socket.cpp
        core/nodes/socket.h
        core/scene/nodescene.cpp
        core/scene/nodescene.h
        ui/canvas/canvasScene.cpp
        ui/canvas/canvasScene.h
        ui/canvas/helper.cpp
        ui/mainwindow/mainWindow.cpp
        ui/mainwindow/mainWindow.h
        core/widgets/widgets.cpp
        core/widgets/widgets.h
        ui/graphics/nodeGraphics.cpp
        ui/graphics/nodeGraphics.h
        ui/graphics/edgeGraphics.cpp
        ui/graphics/edgeGraphics.h
        ui/graphics/socketGraphics.cpp
        ui/graphics/socketGraphics.h
        ui/canvas/canvasview.cpp
        ui/canvas/canvasview.h
        ui/graphics/cutlineGraphics.cpp
        ui/graphics/cutlineGraphics.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DiverseXEngine
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DiverseXEngine APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DiverseXEngine SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DiverseXEngine
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DiverseXEngine PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DiverseXEngine)
endif()
set_target_properties(DiverseXEngine PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DiverseXEngine
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DiverseXEngine)
endif()
