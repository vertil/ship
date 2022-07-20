
cmake_minimum_required(VERSION 3.18)
project(ship_project CXX C)

# Add SDL2 CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)


#find_package(SDL2 REQUIRED)

#IF(${SDL2_FOUND})
#    MESSAGE("FOUND")
#ELSE(${SDL2_FOUND})
#    MESSAGE("can't FOUND")
#ENDIF(${SDL2_FOUND})

#find_package(Freetype REQUIRED)

#find_package(SDL_ttf REQUIRED)

IF(${SDLTTF_FOUND})
    MESSAGE("Freetype FOUND")
ELSE(${SDLTTF_FOUND})
    MESSAGE("can't FOUND Freetype")
ENDIF(${SDLTTF_FOUND})

add_library(glad SHARED
            KHR/khrplatform.h
            glad/glad.h
            glad/glad.c)
target_compile_features(glad PUBLIC cxx_std_17)

add_library(lodepng SHARED
                    lodepng.cpp
                    lodepng.h)
target_compile_features(lodepng PUBLIC cxx_std_17)

add_executable(game_ship

                            main.cpp
                            structs.hxx
                            engine.hxx
                            myimgui.h
                            animation.hxx
                            numbers.hxx
                            )

#sdl2
#target_include_directories(game_ship PRIVATE ${SDL2_INCLUDE_DIRS})
#target_link_libraries(game_ship PRIVATE ${SDL2_LIBRARIES})
#target_link_libraries(game_ship PRIVATE SDL2::SDL2 SDL2::SDL2main)
#sdl-ttf
#target_include_directories(game_ship PRIVATE ${SDL_TTF_INCLUDE_DIRS})
#target_link_libraries(game_ship PRIVATE ${SDL_TTF_LIBRARIES})

# Add SDL2 library
find_package(SDL2 REQUIRED)
target_link_libraries(game_ship PUBLIC SDL2::Main)

# Add SDL2_ttf library
find_package(SDL2_ttf REQUIRED)
target_link_libraries(game_ship PUBLIC SDL2::TTF)

# Add SDL2_image library
find_package(SDL2_image REQUIRED)
target_link_libraries(game_ship PUBLIC SDL2::Image)

target_link_libraries(game_ship PUBLIC imgui)
target_link_libraries(game_ship PRIVATE glad)
target_link_libraries(game_ship PRIVATE lodepng)


target_compile_features(game_ship PUBLIC cxx_std_20)


