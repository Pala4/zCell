cmake_minimum_required(VERSION 3.14)

set(project_name zCellSrvLib)   #project name
set(version 1.0.0.0)            #project version

if (WIN32)
    project(${project_name} VERSION ${version} LANGUAGES CXX)
elseif(UNIX)
    project(${project_name} VERSION ${version})
endif()
message("Building project: " ${PROJECT_NAME} " ver." ${PROJECT_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

set(INCLUDE_DIR "./include")
set(SOURCE_DIR "./src")
message("Include dir: " ${INCLUDE_DIR})
message("Source dir: " ${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)
message("Src project files: " ${SOURCES})

set(prj_shlib_dir "${CMAKE_SOURCE_DIR}/lib")
if(DEFINED cfg_zcell_srv_shlib_dir)
  set(prj_shlib_dir ${cfg_zcell_srv_shlib_dir})
endif(DEFINED cfg_zcell_srv_shlib_dir)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${prj_shlib_dir})

set(prj_stlib_dir "${CMAKE_SOURCE_DIR}/lib")
if(DEFINED cfg_zcell_srv_stlib_dir)
  set(prj_stlib_dir ${cfg_zcell_srv_stlib_dir})
endif(DEFINED cfg_zcell_srv_stlib_dir)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${prj_stlib_dir})
message("Shared library files will moved in: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("Static library files will moved in: " ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(${PROJECT_NAME} PRIVATE ZCELLSRVLIB_LIBRARY)
