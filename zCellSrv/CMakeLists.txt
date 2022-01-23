cmake_minimum_required(VERSION 3.14)

set(project_name zCellSrv)  #project name
set(version 1.0.0.0)        #project version

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

set(prj_bin_dir "${CMAKE_SOURCE_DIR}/bin")
if(DEFINED cfg_zcell_srv_bin_dir)
  set(prj_bin_dir ${cfg_zcell_srv_bin_dir})
endif(DEFINED cfg_zcell_srv_bin_dir)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${prj_bin_dir})
message("Binary files will moved in: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)

if(DEFINED cfg_zcell_srv_lib_dir)
    link_directories(${PROJECT_NAME} ${cfg_zcell_srv_lib_dir})
    target_link_libraries(${PROJECT_NAME} zCellSrvLib)
    message("Target linking with library: " ${cfg_zcell_srv_lib_dir} "/-lzCellSrvLib")
else()
    message(WARNING "Library zCellSrvLib is required!
        See the variable {cfg_zcell_srv_lib_dir} in CMakeLists.txt")
endif(DEFINED cfg_zcell_srv_lib_dir)
if (DEFINED cfg_zcell_srv_lib_include_dir)
    target_include_directories(${PROJECT_NAME} PRIVATE ${cfg_zcell_srv_lib_include_dir})
else()
    message(WARNING "Header files for zCellSrvLib is required!
        See the variable {cfg_zcell_srv_lib_include_dir} in CMakeLists.txt")
endif(DEFINED cfg_zcell_srv_lib_include_dir)
