cmake_minimum_required(VERSION 3.21)

project(LuaBytecode)

set(LUA_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../externals/Lua/src")
set(LUA_BIN_DIR ${CMAKE_BINARY_DIR}/model/lua)
set(LUA_EXECUTABLE "${LUA_SRC_DIR}/luac")

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/lua_sources.lst LUA_FILES_LIST)
string(REPLACE "\n" ";" LUA_FILES_LIST ${LUA_FILES_LIST})
set(LUA_SINGLE_FILES "")

foreach(LUA_FILE IN LISTS LUA_FILES_LIST)
    if(NOT LUA_FILE STREQUAL "")
        list(APPEND LUA_SINGLE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_FILE}")
    endif()
endforeach()

set(ALL_LUA_BYTECODE_FILES "")

foreach(CurrentLuaFile IN LISTS LUA_SINGLE_FILES)
    get_filename_component(CurrentLuaFileBaseName ${CurrentLuaFile} NAME_WE)
    get_filename_component(CurrentLuaFileDir ${CurrentLuaFile} DIRECTORY)    
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${LUA_BIN_DIR} TargetDirectory ${CurrentLuaFileDir})

    set(OutputFile "${TargetDirectory}/${CurrentLuaFileBaseName}.luac")
    list(APPEND ALL_LUA_BYTECODE_FILES ${OutputFile})

    add_custom_command(
        OUTPUT ${OutputFile}
        COMMENT "Compiling Lua file: ${CurrentLuaFile}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TargetDirectory}"
        COMMAND ${LUA_EXECUTABLE} -o ${OutputFile} ${CurrentLuaFile}
        DEPENDS ${CurrentLuaFile}
        VERBATIM
    )    
endforeach()

set(EMBEDDED_LUA_LIST "${CMAKE_CURRENT_SOURCE_DIR}/lua_sources.lst")
set_source_files_properties(
  ${EMBEDDED_LUA_LIST}
  PROPERTIES
  OBJECT_DEPENDS LuaBytecode
)

add_custom_target(LuaBytecode ALL DEPENDS ${ALL_LUA_BYTECODE_FILES} ${EMBEDDED_LUA_LIST})
add_dependencies(LuaBytecode lua_static)

include_directories(${LUA_SRC_DIR})
