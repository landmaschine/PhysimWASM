cmake_minimum_required(VERSION 3.10.0)
project(PhysimWASM VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SORUCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wundef
    -fno-common
    -fstrict-aliasing
    -fstack-protector-strong
  )

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
  endif()
endif()

file(GLOB_RECURSE ENGINE_SOURCES
  "${CMAKE_SOURCE_DIR}/engine/*.cpp"
)

list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/third_party/glm/.*")

file(GLOB GLAD_SOURCES 
  "${CMAKE_SOURCE_DIR}/engine/third_party/glad/src/*.c"
)

file(GLOB MAIN_SOURCES
  "${CMAKE_SOURCE_DIR}/Application/*.cpp"
)

add_executable(PhysimWASM 
  ${ENGINE_SOURCES}
  ${GLAD_SOURCES}
  ${MAIN_SOURCES}
)

target_include_directories(PhysimWASM PRIVATE
  "${CMAKE_SOURCE_DIR}/engine/third_party/SDL3/include"
  "${CMAKE_SOURCE_DIR}/engine/third_party/glad/include"
  "${CMAKE_SOURCE_DIR}/engine/third_party/"
  "${CMAKE_SOURCE_DIR}/engine"
  "${CMAKE_SOURCE_DIR}/"
)

target_link_libraries(PhysimWASM
  "${CMAKE_SOURCE_DIR}/engine/third_party/SDL3/SDL3.lib"
)

add_custom_command(TARGET PhysimWASM POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/engine/third_party/SDL3/SDL3.dll"
    "$<TARGET_FILE_DIR:PhysimWASM>/SDL3.dll"
)
