cmake_minimum_required(VERSION 3.6)

set(APP_NAME coadcpp)

project(${APP_NAME})

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../_cocos2d-x-4.0)

set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)

add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos
        ${ENGINE_BINARY_PATH}/cocos/core)

set(GAME_SOURCE)

set(GAME_HEADER)

set(GAME_RES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/resources)

list(APPEND GAME_HEADER
        AppDelegate.h
        GameLayer.h
        Number.h)

list(APPEND GAME_SOURCE
        jnimain.cpp
        AppDelegate.cpp
        GameLayer.cpp
        Number.cpp)

set(ALL_CODE_FILES ${GAME_HEADER}
        ${GAME_SOURCE})

add_library(${APP_NAME} SHARED ${ALL_CODE_FILES})

add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android
        ${ENGINE_BINARY_PATH}/cocos/platform)

target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive
        cocos2d
        android
        log)

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

setup_cocos_app_config(${APP_NAME})
