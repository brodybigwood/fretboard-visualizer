cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json for Clangd")

project(guitarApp)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 17)

# Enable AddressSanitizer
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

add_compile_definitions(_DEBUG)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

add_executable(app ${SOURCES})
target_compile_definitions(app PRIVATE _DEBUG)

target_link_libraries(app PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)


if(EMSCRIPTEN)
  set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/html/"
    SUFFIX ".html"
    LINK_FLAGS "-sEXPORTED_FUNCTIONS=[\"_main\",\"_malloc\",\"_free\"] -sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"] -sUSE_SDL=3 -sASSERTIONS=1"
  )
endif()



add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)

add_dependencies(app CopyAssets)

