if(${CMAKE_BUILD_TYPE} STREQUAL  "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file bin/asset/DejaVuSans.ttf -DCMAKE_EXPORT_COMPILE_COMMANDS=True  -s SOCKET_DEBUG=1 -s USE_PTHREADS   -fdebug-compilation-dir='../..' -fexceptions -s ASYNCIFY=1  --emit-symbol-map --tracing --bind -g -O0 -std=c++20 -s  -s WASM=1")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DCMAKE_EXPORT_COMPILE_COMMANDS=True -lwebsocket.js -s SOCKET_DEBUG=1 -s USE_PTHREADS  -fdebug-compilation-dir='../..' -fexceptions  -s ASYNCIFY=1 --emit-symbol-map --bind --tracing -g -O0  -fcoroutines-ts -std=c++20 -s  -s WASM=1" )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file bin/asset/DejaVuSans.ttf -DCMAKE_EXPORT_COMPILE_COMMANDS=True -std=c++20 -s USE_PTHREADS")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_PTHREADS -DCMAKE_EXPORT_COMPILE_COMMANDS=True -fcoroutines-ts -std=c++20 -lwebsocket.js")
endif()
cmake_minimum_required(VERSION 3.17)
project(project VERSION 1.0.0
  DESCRIPTION project
  LANGUAGES CXX
  ) 
cmake_policy(SET CMP0015 NEW)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

########## enable cache system
include(cmake/Cache.cmake)

########## link against the libs to apply the configurations to the target
# compiler warnings
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
# compiler options
set(PROJECT_OPTIONS project_options)


add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
# target_precompile_headers(${PROJECT_OPTIONS} INTERFACE 
# )
#set(CMAKE_CXX_STANDARD 20)

# include(cmake/Sanitizers.cmake)
# enable_sanitizers(${PROJECT_OPTIONS})

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake) 
 
######## conan setup
set(BUILD_WASM TRUE)
set(CMAKE_EXECUTABLE_SUFFIX ".html")
IF(BUILD_WASM)
SET(CONAN_DISABLE_CHECK_COMPILER ON)
ENDIF(BUILD_WASM)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


set(TARGET_NAME project)
add_executable(${TARGET_NAME})


#copy assets into bin so emscripten finds them
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/asset/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/asset)

add_custom_command(
        TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/asset/ 
                ${CMAKE_CURRENT_BINARY_DIR}/bin/asset
                )
######## add sub dir
add_subdirectory(src) 


####### link

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${TARGET_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


target_link_libraries(${TARGET_NAME} PRIVATE
  ${PROJECT_WARNINGS}
  ${PROJECT_OPTIONS}
  CONAN_PKG::sml
  CONAN_PKG::pipes
  CONAN_PKG::range-v3
  CONAN_PKG::fmt
  CONAN_PKG::magic_enum
  CONAN_PKG::game_01_shared_class
  CONAN_PKG::durak
  CONAN_PKG::confu_json
  CONAN_PKG::magnum-integration
  CONAN_PKG::boost
)





####### create executable
OPTION(BUILD_TESTS "enable tests" OFF)
IF(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test) 
ENDIF(BUILD_TESTS)

# add main after  add_subdirectory(test) so we can take the sources from ${TARGET_NAME} and dont have to remove main.cxx from that list
target_sources(${TARGET_NAME} PRIVATE 
"main.cxx"
)
 
if (CMAKE_BINARY_DIR)
target_compile_definitions(${TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
endif()

target_compile_definitions(${TARGET_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

####### include what you use
#find_program(iwyu_path NAMES include-what-you-use iwyu)
#set_property(TARGET ${TARGET_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})


#print all variables used in cmake
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

