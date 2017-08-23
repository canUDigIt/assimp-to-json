cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(assimp-to-json VERSION 0.1 LANGUAGES CXX)

find_package(assimp REQUIRED)

add_executable(atj main.cpp)

target_include_directories(atj 
    PRIVATE
        ${assimp_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

target_link_libraries(atj
    PRIVATE ${ASSIMP_LIBRARIES})

target_compile_features(atj
    PRIVATE 
        cxx_lambdas
        cxx_lambda_init_captures
        cxx_variadic_templates)

if(${CMAKE_GENERATOR} STREQUAL "Ninja")
    message(STATUS "Using the Ninja generator")
    target_compile_options(atj
        PUBLIC -fdiagnostics-color=always)
endif()
