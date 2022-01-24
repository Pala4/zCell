cmake_minimum_required(VERSION 3.14)

set(project_name zCell)  #project name
set(version 1.0.0.0)     #project version
project(${project_name} VERSION ${version})

set(cfg_zcell_srv_shlib_dir "${CMAKE_SOURCE_DIR}/zCellBin")
set(cfg_zcell_srv_stlib_dir "${CMAKE_SOURCE_DIR}/zCellSrvLib/lib")
add_subdirectory(./zCellSrvlib)

set(cfg_zcell_srv_bin_dir "${CMAKE_SOURCE_DIR}/zCellBin")
set(cfg_zcell_srv_lib_dir "${CMAKE_SOURCE_DIR}/zCellSrvLib/lib")
set(cfg_zcell_srv_lib_include_dir "${CMAKE_SOURCE_DIR}/zCellSrvLib/lib/include")
set(cfg_zcell_srv_lib_src_dir "${CMAKE_SOURCE_DIR}/zCellSrvLib/src")

#copying lib api interface includes to directory 'cfg_zcell_srv_lib_include_dir'
file(GLOB_RECURSE lib_includes
    "${cfg_zcell_srv_lib_src_dir}/*.h"
)

#pars the path to the *.h library files to get its subdirectories and file name
foreach(lib_inc_with_parent_dir ${lib_includes})
    #exlude ${CMAKE_SOURCE_DIR}/zCellSrvLib/src from path
    string(REPLACE ${cfg_zcell_srv_lib_src_dir} "" lib_inc ${lib_inc_with_parent_dir})

    #find the directory in the following view: "/dir/dir1/../dirN/"
    #find pos at first "/"
    string(FIND ${lib_inc} "/" firs_sl_ind)
    #find pos at last "/"
    string(FIND ${lib_inc} "/" last_sl_ind REVERSE)

    #get the dir between first and last slashes
    string(SUBSTRING ${lib_inc} ${firs_sl_ind} ${last_sl_ind} sub_dir)
    #get file_name.h from last slash to str end
    string(SUBSTRING ${lib_inc} ${last_sl_ind} -1 file_name)

    #making destination dir
    set(include_dir ${cfg_zcell_srv_lib_include_dir}${sub_dir})
    message("Making library include dir: " ${include_dir})
    file(MAKE_DIRECTORY ${include_dir})

    #copying include files to destination
    file(COPY ${lib_inc_with_parent_dir} DESTINATION ${include_dir})
    message("copying include file: [" ${lib_inc_with_parent_dir} "] to dir: [" ${include_dir} "]")
endforeach(lib_inc_with_parent_dir ${lib_includes})

add_subdirectory(./zCellSrv)
