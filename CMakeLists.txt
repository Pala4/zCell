cmake_minimum_required(VERSION 3.14)

set(project_name zCell)  #project name
set(version 1.0.0.0)     #project version
project(${project_name} VERSION ${version})

set(cfg_zcell_srv_shlib_dir "${CMAKE_SOURCE_DIR}/zCellBin")
set(cfg_zcell_srv_stlib_dir "${CMAKE_SOURCE_DIR}/zCellSrvLib/lib")
add_subdirectory(./zCellSrvlib)

set(cfg_zcell_srv_bin_dir "${CMAKE_SOURCE_DIR}/zCellBin")
set(cfg_zcell_srv_lib_dir ${CMAKE_SOURCE_DIR}/zCellSrvLib/lib)
set(cfg_zcell_srv_lib_include_dir ${CMAKE_SOURCE_DIR}/zCellSrvLib/include)
add_subdirectory(./zCellSrv)
