cmake_minimum_required(VERSION 3.21)

file(STRINGS "VERSION.txt" OPENVSM_VERSION)
message(STATUS "VERSION: ${OPENVSM_VERSION}")

project(VSM_DLL VERSION ${OPENVSM_VERSION} LANGUAGES C CXX)

if(NOT CMAKE_GENERATOR)
    set(CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
endif()

set(BUILD_SHARED_LIBS ON)
option(BUILD_TESTS "Enable building tests" OFF)

set(SRCDIR cpp)

set(DLL_SRC
  ${SRCDIR}/dllmain.cc
  ${SRCDIR}/model.cc
  ${SRCDIR}/luabind/device/bus.cc
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
set_target_properties(VSM_DLL PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(VSM_DLL PRIVATE
  OPENVSM_VERSION="${OPENVSM_VERSION}"
  $<$<CONFIG:Debug>:OPENVSM_DEBUG>
  $<$<NOT:$<CONFIG:Debug>>:OPENVSM_RELEASE>
)

target_compile_options(VSM_DLL PRIVATE
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/EHsc>
)

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
if(BUILD_TESTS)
  add_executable(native_bus_test
    tests/native_bus_test.cc
    ${SRCDIR}/luabind/device/bus.cc
  )
  target_include_directories(native_bus_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(native_bus_test PRIVATE lua_static tinyLog)
  if(MSVC)
    target_compile_options(native_bus_test PRIVATE /EHsc)
  endif()

  add_test(NAME native_bus COMMAND native_bus_test)
endif()
