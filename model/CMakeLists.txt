cmake_minimum_required(VERSION 3.21)

file(STRINGS "VERSION.txt" OPENVSM_VERSION)
message(STATUS "VERSION: ${OPENVSM_VERSION}")

project(VSM_DLL VERSION ${OPENVSM_VERSION} LANGUAGES C CXX)

if(NOT CMAKE_GENERATOR)
    set(CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
if (_cpp_latest_flag_supported)
  add_compile_options("/std:c++latest")
  add_compile_options("/EHsc")
endif()

set(BUILD_SHARED_LIBS ON)
option(BUILD_TESTS "Enable building tests" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DOPENVSM_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG  "-ggdb -Og")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Og")    
else()
    add_definitions(-DOPENVSM_RELEASE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")    
endif()

add_definitions(-DOPENVSM_VERSION="${OPENVSM_VERSION}")

set(CFLAGS "${CFLAGS} ${OPTIM}")
set(CXXFLAGS "${CXXFLAGS} ${OPTIM}")

set(SRCDIR cpp)

set(DLL_SRC
  ${SRCDIR}/dllmain.cc
  ${SRCDIR}/model.cc
  ${SRCDIR}/luabind/device/pin.cc
  ${SRCDIR}/luabind/lua_facade.cc
  ${SRCDIR}/luabind/lua_script_executor.cc
  ${SRCDIR}/luabind/bytecode_loader.cc
  ${SRCDIR}/luabind/resources_map.cc
)

# Misc Block
set(MISC_SRCS
  ${SRCDIR}/dll.rc
)

# Combine all the source blocks
set(CPPSRC
  ${DLL_SRC}
  ${MISC_SRCS}
)


add_library(VSM_DLL SHARED ${CPPSRC})
set_property(TARGET VSM_DLL PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(VSM_DLL PROPERTIES LINKER_LANGUAGE C)

target_compile_options(VSM_DLL PRIVATE -m32)
target_link_options(VSM_DLL PRIVATE -m32)

set_target_properties(VSM_DLL PROPERTIES PREFIX "")
set_target_properties(VSM_DLL PROPERTIES OUTPUT_NAME "openvsm")

include_directories(${SRCDIR} include cpp/luabind)

set(CPP_SOURCE_WITH_EMBEDDED_LUA "${SRCDIR}/luabind/resources_map.cc")
set_source_files_properties(
  ${CPP_SOURCE_WITH_EMBEDDED_LUA}
  PROPERTIES
  OBJECT_DEPENDS LuaBytecode
)

set(SOURCE_LIST_WITH_EMBEDDED_LUA "${CMAKE_CURRENT_SOURCE_DIR}/lua/lua_sources.lst")
set_source_files_properties(
  ${SOURCE_LIST_WITH_EMBEDDED_LUA}
  PROPERTIES
  OBJECT_DEPENDS LuaBytecode
)

add_dependencies(VSM_DLL lua_static)
add_dependencies(VSM_DLL LuaBytecode)

target_link_libraries(VSM_DLL PRIVATE  
lua_static
tinyLog
)