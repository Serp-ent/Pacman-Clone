cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find SDL2_image
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_LIBRARIES})
set(SDL2_IMAGE_LIBRARIES "/usr/lib/libSDL2_image.so")

# Find SDL2_ttf
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})
set(SDL2_TTF_LIBRARY "/usr/lib/libSDL2_ttf.so")

file(GLOB SOURCE_FILES "./src/*.cpp")

add_executable(game ${SOURCE_FILES})

target_link_libraries(game ${SDL2_LIBRARIES})

# Link SDL2_image library
target_link_libraries(game ${SDL2_IMAGE_LIBRARIES})
# Link SDL2_ttf library
target_link_libraries(game ${SDL2_TTF_LIBRARY})

# Link SDL2_image library
target_link_libraries(game ${SDL2_IMAGE_LIBRARIES})
# Link SDL2_ttf library
target_link_libraries(game ${SDL2_TTF_LIBRARY} ${SDL2_TTF_MAIN_LIBRARY})

message("SDL2_ttf library: ${SDL2_TTF_LIBRARY}")
message("SDL2_ttf main library: ${SDL2_TTF_MAIN_LIBRARY}")
message("SDL2_image library: " ${SDL2_IMAGE_LIBRARIES})

# Set the output directory for the executable
set_target_properties(game PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                      ${CMAKE_CURRENT_SOURCE_DIR}/build)

# Define variables for texture and font directories
set(TEXTURE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/textures")
set(FONT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")
set(LEVEL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/levels")

# Copy texture and font directories to build directory
file(COPY ${TEXTURE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${FONT_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${LEVEL_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
