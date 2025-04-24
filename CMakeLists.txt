cmake_minimum_required(VERSION 3.10.0)
project(PhysimWASM VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

option(BUILD_WASM "Build for WebAssembly" OFF)

if(BUILD_WASM)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  
  set(WASM_COMPILER_FLAGS
    -sUSE_SDL=2
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
    -sUSE_WEBGL2=1
    -sFULL_ES3=1
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']
    --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets
  )
  
  set(WASM_LINK_FLAGS
    ${WASM_COMPILER_FLAGS}
    -sEXPORTED_FUNCTIONS=['_main','_updateCanvasSize']
    -sENVIRONMENT=web
    -sASYNCIFY=1
    -O2
    --shell-file ${CMAKE_SOURCE_DIR}/shell.html
  )
  
  add_compile_definitions(FORCE_GLES=1)
  
  foreach(flag ${WASM_COMPILER_FLAGS})
    add_compile_options(${flag})
  endforeach()

  string(REPLACE ";" " " WASM_LINK_FLAGS_STR "${WASM_LINK_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WASM_LINK_FLAGS_STR}")
  
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/web)
else()
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
endif()

configure_file(${CMAKE_SOURCE_DIR}/engine/third_party/stb_image/stb_image.h ${CMAKE_BINARY_DIR}/stb_image.h COPYONLY)

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

if(BUILD_WASM)
else()
  target_link_libraries(PhysimWASM
    "${CMAKE_SOURCE_DIR}/engine/third_party/SDL3/SDL3.lib"
  )

  if(WIN32)
    add_custom_command(TARGET PhysimWASM POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/engine/third_party/SDL3/SDL3.dll"
        "$<TARGET_FILE_DIR:PhysimWASM>/SDL3.dll"
    )
  endif()
endif()

