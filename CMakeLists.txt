
# Common

# we only need boost for websocketpp
find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(COMMON_SOURCES
        src/project/project.cpp src/project/stroke.cpp src/project/graphic.cpp
        src/protocol/msg.cpp
        src/common/common.cpp src/common/scanner.cpp)

add_subdirectory(libs/glm)

# Client

project(anim_client)

set(CLIENT_SOURCES
        src/client/main.cpp src/client/editor.cpp src/client/tool.cpp src/client/panel.cpp
        src/client/panels/scene.cpp src/client/panels/assets.cpp src/client/panels/debug.cpp
        ${COMMON_SOURCES}
        src/platform/common.cpp
        src/render/mesh.cpp src/render/shader.cpp src/render/builder.cpp src/render/framebuffer.cpp src/render/scene.cpp src/render/camera.cpp
        src/client/keychain.cpp
        src/client/action.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_tables.cpp libs/imgui/imgui_widgets.cpp libs/imgui/backends/imgui_impl_glfw.cpp libs/imgui/backends/imgui_impl_opengl3.cpp)

if(DEFINED EMSCRIPTEN)
    set(CLIENT_SOURCES ${CLIENT_SOURCES} src/platform/web.cpp)
else()
    set(CLIENT_SOURCES ${CLIENT_SOURCES} src/platform/desktop.cpp)
endif()

add_executable(anim_client ${CLIENT_SOURCES})
target_include_directories(anim_client PRIVATE libs/imgui)

if(DEFINED EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sEXPORTED_RUNTIME_METHODS=stringToNewUTF8 -lGL -lwebsocket.js -s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1")
else()
    add_subdirectory(libs/glfw)
    target_link_libraries(anim_client glfw)
    add_subdirectory(libs/glew)
    target_link_libraries(anim_client libglew_static)
    target_include_directories(anim_client PRIVATE libs/websock)
    target_link_libraries(anim_client ${Boost_LIBRARIES})
    target_include_directories(anim_client PRIVATE ${Boost_INCLUDE_DIR})
endif()

target_link_libraries(anim_client glm)

# Server

project(anim_server)

add_executable(anim_server
    src/server/main.cpp
    src/server/server.cpp src/server/keysmith.cpp
    ${COMMON_SOURCES})

target_include_directories(anim_server PRIVATE libs/websock)
target_link_libraries(anim_server ${Boost_LIBRARIES})
target_include_directories(anim_server PRIVATE ${Boost_INCLUDE_DIR})
target_compile_definitions(anim_server PRIVATE SERVER)
