cmake_minimum_required(VERSION 3.10)
project(Wordle-Cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    add_compile_options(-stdlib=libc++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++")
endif()

find_package(CURL REQUIRED)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/raylib)
set(SOURCES
    src/main.cpp        
    src/wordle.cpp
    src/button.cpp
    src/keyboard.cpp
    src/timer.cpp
    src/wordleDraw.cpp
    src/wordleUsername.cpp
    src/textBox.cpp
    src/drawStats.cpp
    src/leaderboard.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE vendor/raylib/src)


target_link_libraries(${PROJECT_NAME} raylib CURL::libcurl)


if(APPLE) 
    target_link_libraries(${PROJECT_NAME} c++) 
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")
endif()