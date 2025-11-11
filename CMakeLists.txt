cmake_minimum_required(VERSION 3.28)
project(GraphBuilder LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

if (WIN32)
include_directories(./include/imgui-1.91.1 ./include/imgui-sfml-2.6.1)
else()
include_directories(${CMAKE_SOURCE_DIR}/include/imgui-1.91.1 ${CMAKE_SOURCE_DIR}/include/imgui-sfml-2.6.1)
endif()
add_executable(main src/main.cpp src/imgui_draw.cpp src/imgui_tables.cpp src/imgui_widgets.cpp src/imgui-SFML.cpp src/imgui.cpp)
target_link_libraries(main PRIVATE sfml-graphics)

message("${SFML_VERSION_MAJOR}")

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
