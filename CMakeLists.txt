cmake_minimum_required(VERSION 3.16)

project(DiverseXEngine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)

set(PROJECT_SOURCES
        main.cpp
        main.ui
        resources.qrc
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
        ui/graphics/InfoButton.cpp
        ui/graphics/InfoButton.h
        ui/graphics/edgeGraphics.cpp
        ui/graphics/edgeGraphics.h
        ui/graphics/socketGraphics.cpp
        ui/graphics/socketGraphics.h
        ui/graphics/canvasNodeGraphis.cpp
        ui/graphics/canvasNodeGraphis.h
        ui/canvas/canvasview.cpp
        ui/canvas/canvasview.h
        ui/graphics/cutlineGraphics.cpp
        ui/graphics/cutlineGraphics.h
        core/serialization/serializator.cpp
        core/serialization/serializator.h
        core/noderegistry/noderegister.cpp
        core/noderegistry/noderegister.h
        core/noderegistry/functions.cpp
        core/noderegistry/canvasNode.cpp
        core/noderegistry/canvasNode.h
        core/noderegistry/function/functionNode.cpp
        core/noderegistry/function/functionNode.h
        core/noderegistry/function/conditionNode.cpp
        core/noderegistry/function/conditionNode.h
        core/noderegistry/function/ifElseNode.cpp
        core/noderegistry/function/ifElseNode.h
        core/noderegistry/inout/inNode.cpp
        core/noderegistry/inout/inoutNode.h
        core/noderegistry/inout/outNode.cpp
        core/noderegistry/inout/constantNode.cpp
        # header is included automatically by AUTOUIC/AUTOMOC, but list it for clarity
        core/noderegistry/inout/constantNode.h
        ui/canvas/editorWindow.cpp
        ui/canvas/editorWindow.h
        ui/canvas/syshighligh.cpp
        ui/canvas/syshighligh.h
        core/codegeneration/codeTemplateManager.cpp
        core/codegeneration/codeTemplateManager.h
        ui/canvas/load_from.cpp
        ui/canvas/open_from.cpp
        ui/canvas/open_load.h
        core/noderegistry/arithmetic/mathNode.cpp
        core/noderegistry/arithmetic/mathNode.h
        core/noderegistry/permutation/Permutation.cpp
        core/noderegistry/permutation/Permutation.h
        core/noderegistry/bitOps/BitOp.cpp
        core/noderegistry/bitOps/BitOp.h
        core/noderegistry/inout/printNode.cpp
        core/noderegistry/inout/printNode.h
        core/noderegistry/matrix/matrixNode.cpp
        core/noderegistry/matrix/matrixNode.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DiverseXEngine
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(DiverseXEngine SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(DiverseXEngine
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DiverseXEngine PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)

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
