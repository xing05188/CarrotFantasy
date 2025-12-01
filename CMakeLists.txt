#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

set(APP_NAME CarrotFantasy)

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test CarrotFantasy into root project
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/core/AppDelegate.cpp
     Classes/scenes/advantureScene.cpp
     Classes/scenes/BaseLevelScene.cpp
     Classes/scenes/controllers/SkillController.cpp
     Classes/scenes/controllers/GameMenuController.cpp
     Classes/scenes/controllers/GameSpeedController.cpp
     Classes/scenes/controllers/CountDownController.cpp
     Classes/entities/Carrot/Carrot.cpp
     Classes/entities/Carrot/CarrotState.cpp
     Classes/core/GameManager.cpp
     Classes/core/subsystem/MonsterManager.cpp
     Classes/core/subsystem/PathManager.cpp
     Classes/core/subsystem/CarrotManager.cpp
     Classes/core/subsystem/GameSaveLoader.cpp
     Classes/core/subsystem/GameStateChecker.cpp
     Classes/core/subsystem/MoneySystem.cpp
     Classes/core/StorageService/StorageService.cpp
     Classes/scenes/menuScene.cpp
     Classes/entities/Monster/Monster.cpp
     Classes/config/MonsterConfigs.cpp
     Classes/audio/music.cpp
     Classes/entities/Obstacle/Obstacle.cpp
     Classes/entities/Obstacle/ObstacleFactory.cpp
     Classes/scenes/seeScene.cpp
     Classes/scenes/setScene.cpp
     Classes/scenes/themeScene.cpp
     Classes/entities/Tower/Tower.cpp
     Classes/entities/Tower/TowerFactory.cpp
     Classes/entities/Tower/AttackStrategy.cpp
     Classes/ui/widgets/MoneyHud.cpp
     Classes/core/state/GameState.cpp
     Classes/core/state/PausedState.cpp
     Classes/core/state/MenuState.cpp
     Classes/core/state/LevelSelectState.cpp
     Classes/core/state/LoadingState.cpp
     Classes/core/state/LevelState.cpp
     Classes/core/state/GameOverState.cpp
     Classes/core/state/GameFlowController.cpp
     Classes/core/state/GameFlowProvider.cpp
     Classes/entities/Monster/MonsterFactory.cpp
     )

list(APPEND GAME_HEADER
     Classes/core/AppDelegate.h
     Classes/scenes/advantureScene.h
     Classes/scenes/BaseLevelScene.h
     Classes/scenes/controllers/SkillController.h
     Classes/scenes/controllers/GameMenuController.h
     Classes/scenes/controllers/GameSpeedController.h
     Classes/scenes/controllers/CountDownController.h
     Classes/entities/Carrot/Carrot.h
     Classes/entities/Carrot/CarrotState.h
     Classes/core/GameManager.h
     Classes/core/subsystem/MonsterManager.h
     Classes/core/subsystem/MonsterTypes.h
     Classes/core/subsystem/PathManager.h
     Classes/core/subsystem/CarrotManager.h
     Classes/core/subsystem/GameSaveLoader.h
     Classes/core/subsystem/GameStateChecker.h
     Classes/core/subsystem/MoneySystem.h
     Classes/core/StorageService/StorageService.h
     Classes/scenes/menuScene.h
     Classes/entities/Monster/Monster.h
     Classes/config/MonsterConfigs.h
     Classes/audio/music.h
     Classes/config/obpos.h
     Classes/entities/Obstacle/Obstacle.h
     Classes/entities/Obstacle/ObstacleFactory.h
     Classes/scenes/seeScene.h
     Classes/scenes/setScene.h
     Classes/scenes/themeScene.h
     Classes/entities/Tower/Tower.h
     Classes/entities/Tower/TowerFactory.h
     Classes/entities/Tower/AttackStrategy.h
     Classes/ui/widgets/MoneyHud.h
     Classes/core/state/GameState.h
     Classes/core/state/PausedState.h
     Classes/core/state/MenuState.h
     Classes/core/state/LevelSelectState.h
     Classes/core/state/LoadingState.h
     Classes/core/state/LevelState.h
     Classes/core/state/GameOverState.h
     Classes/core/state/GameStateProvider.h
     Classes/core/state/GameFlowController.h
     Classes/core/state/GameFlowProvider.h
     Classes/gameplay/events/CarrotEvents.h
     Classes/entities/Monster/MonsterFactory.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE Classes/core
        PRIVATE Classes/scenes
        PRIVATE Classes/entities
        PRIVATE Classes/ui
        PRIVATE Classes/config
        PRIVATE Classes/audio
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")

    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
