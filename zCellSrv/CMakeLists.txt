cmake_minimum_required(VERSION 3.14)

set(project_name zCellSrv)  #project name
set(version 1.0.0.0)        #project version

if (WIN32)
    project(${project_name} VERSION ${version} LANGUAGES CXX)
elseif(UNIX)
    project(${project_name} VERSION ${version})
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

#=================== INCLUSION OF Project Files ====================#
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

#=================== SETUP EXECTUABLE ====================#
# Enable debug logging on RELWITHDEBINFO configuration
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

# Add the executable
if (WIN32) 
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
elseif(UNIX)
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# Add the target includes for project
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

#===================== LINKING LIBRARIES =======================#
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)

#===================== STATUS MESSAGES =======================#
message("Building project " ${PROJECT_NAME} " ver." ${PROJECT_VERSION})
