cmake_minimum_required(VERSION 3.16)

project(MarsRoverTelemetrySystem
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rover
    src/main.cpp
    src/Measurement.cpp
    src/MSLAssertionLoader.cpp
    src/Navigation.cpp
    src/PerseveranceBuilder.cpp
    src/Planet.cpp
    src/Proposition.cpp
    src/Ratiocinator.cpp
    src/Robot.cpp
    src/Sample.cpp
    src/SherlocClassifier.cpp
    src/TemperatureStats.cpp
    src/TruthReconciler.cpp
    src/Unit.cpp
)

target_include_directories(rover PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_definitions(rover PRIVATE _USE_MATH_DEFINES)
endif()