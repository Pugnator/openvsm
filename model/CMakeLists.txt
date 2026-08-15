cmake_minimum_required(VERSION 3.21)

file(STRINGS "VERSION.txt" OPENVSM_VERSION)
message(STATUS "VERSION: ${OPENVSM_VERSION}")

project(VSM_DLL VERSION ${OPENVSM_VERSION} LANGUAGES C CXX)

if("${PROJECT_VERSION_PATCH}" STREQUAL "")
  set(PROJECT_VERSION_PATCH 0)
endif()
if("${PROJECT_VERSION_TWEAK}" STREQUAL "")
  set(PROJECT_VERSION_TWEAK 0)
endif()

configure_file(
  include/resources.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/generated/resources.h"
  @ONLY
)

set(BUILD_SHARED_LIBS ON)
option(BUILD_TESTS "Enable building tests" OFF)

set(SRCDIR cpp)

set(DLL_SRC
  ${SRCDIR}/dllmain.cc
  ${SRCDIR}/active_model.cc
  ${SRCDIR}/model.cc
  ${SRCDIR}/luabind/device/bus.cc
  ${SRCDIR}/model_script_path.cc
  ${SRCDIR}/luabind/device/lua_event_dispatcher.cc
  ${SRCDIR}/luabind/device/pin.cc
  ${SRCDIR}/luabind/lua_callback.cc
  ${SRCDIR}/luabind/device/pin_timing.cc
  ${SRCDIR}/luabind/graphics_lua_api.cc
  ${SRCDIR}/luabind/lua_facade.cc
  ${SRCDIR}/luabind/lua_script_executor.cc
  ${SRCDIR}/luabind/vdm_lua_api.cc
  ${SRCDIR}/luabind/vsm_lua_api.cc
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
target_include_directories(VSM_DLL PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

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
    ${SRCDIR}/luabind/device/lua_event_dispatcher.cc
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

  add_executable(pin_registration_stack_test tests/pin_registration_stack_test.cc)
  target_include_directories(pin_registration_stack_test PRIVATE ${SRCDIR})
  target_link_libraries(pin_registration_stack_test PRIVATE VSM_DLL lua_static)
  add_test(NAME pin_registration_stack COMMAND pin_registration_stack_test)

  add_executable(script_executor_ownership_test
    tests/script_executor_ownership_test.cc
    ${SRCDIR}/luabind/lua_script_executor.cc
  )
  target_include_directories(script_executor_ownership_test PRIVATE ${SRCDIR}/luabind)
  target_link_libraries(script_executor_ownership_test PRIVATE lua_static tinyLog)
  add_test(NAME script_executor_ownership COMMAND script_executor_ownership_test)

  add_executable(simulation_stack_test tests/simulation_stack_test.cc)
  target_include_directories(simulation_stack_test PRIVATE ${SRCDIR})
  target_link_libraries(simulation_stack_test PRIVATE VSM_DLL lua_static)
  add_test(NAME simulation_stack COMMAND simulation_stack_test)

  add_executable(model_deletion_test tests/model_deletion_test.cc)
  target_include_directories(model_deletion_test PRIVATE ${SRCDIR})
  target_link_libraries(model_deletion_test PRIVATE VSM_DLL lua_static)
  add_test(NAME model_deletion COMMAND model_deletion_test)

  add_executable(model_authorization_test tests/model_authorization_test.cc)
  target_include_directories(model_authorization_test PRIVATE ${SRCDIR})
  target_link_libraries(model_authorization_test PRIVATE VSM_DLL lua_static)
  add_test(NAME model_authorization COMMAND model_authorization_test)

  add_executable(model_script_path_test
    tests/model_script_path_test.cc
    ${SRCDIR}/model_script_path.cc
  )
  target_include_directories(model_script_path_test PRIVATE ${SRCDIR})
  target_link_libraries(model_script_path_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(model_script_path_test PRIVATE /EHsc)
  endif()

  add_test(NAME model_script_path COMMAND model_script_path_test)

  add_executable(lua_lifecycle_test
    tests/lua_lifecycle_test.cc
    ${SRCDIR}/luabind/lua_callback.cc
  )
  target_include_directories(lua_lifecycle_test PRIVATE ${SRCDIR})
  target_link_libraries(lua_lifecycle_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(lua_lifecycle_test PRIVATE /EHsc)
  endif()

  add_test(NAME lua_lifecycle COMMAND lua_lifecycle_test)

  add_executable(pin_timing_test
    tests/pin_timing_test.cc
    ${SRCDIR}/luabind/device/pin_timing.cc
  )
  target_include_directories(pin_timing_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(pin_timing_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(pin_timing_test PRIVATE /EHsc)
  endif()

  add_test(NAME pin_timing COMMAND pin_timing_test)

  add_executable(vsm_lua_api_test
    tests/vsm_lua_api_test.cc
    ${SRCDIR}/luabind/vsm_lua_api.cc
  )
  target_include_directories(vsm_lua_api_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(vsm_lua_api_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(vsm_lua_api_test PRIVATE /EHsc)
  endif()

  add_test(NAME vsm_lua_api COMMAND vsm_lua_api_test)

  add_executable(lua_test_ic_test tests/lua_test_ic_test.cc)
  target_link_libraries(lua_test_ic_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(lua_test_ic_test PRIVATE /EHsc)
  endif()

  add_test(
    NAME lua_test_ic
    COMMAND lua_test_ic_test "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/test_ic.lua"
  )

  add_executable(object_callback_test
    tests/object_callback_test.cc
    ${SRCDIR}/luabind/device/lua_event_dispatcher.cc
  )
  target_include_directories(object_callback_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(object_callback_test PRIVATE lua_static tinyLog)
  if(MSVC)
    target_compile_options(object_callback_test PRIVATE /EHsc)
  endif()

  add_test(NAME object_callback COMMAND object_callback_test)

  add_executable(uart_module_test tests/uart_module_test.cc)
  target_link_libraries(uart_module_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(uart_module_test PRIVATE /EHsc)
  endif()

  add_test(
    NAME uart_module
    COMMAND uart_module_test "${CMAKE_CURRENT_SOURCE_DIR}/lua/modules/uart.lua"
  )

  add_executable(vdm_lua_api_test
    tests/vdm_lua_api_test.cc
    ${SRCDIR}/luabind/vdm_lua_api.cc
  )
  target_include_directories(vdm_lua_api_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(vdm_lua_api_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(vdm_lua_api_test PRIVATE /EHsc)
  endif()

  add_test(NAME vdm_lua_api COMMAND vdm_lua_api_test)

  add_executable(vdm_control_module_test
    tests/vdm_control_module_test.cc
    ${SRCDIR}/luabind/vdm_lua_api.cc
  )
  target_include_directories(vdm_control_module_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(vdm_control_module_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(vdm_control_module_test PRIVATE /EHsc)
  endif()

  add_test(
    NAME vdm_control_module
    COMMAND vdm_control_module_test "${CMAKE_CURRENT_SOURCE_DIR}/lua/modules/vdm_control.lua"
  )

  add_executable(graphics_lua_api_test
    tests/graphics_lua_api_test.cc
    ${SRCDIR}/luabind/graphics_lua_api.cc
  )
  target_include_directories(graphics_lua_api_test PRIVATE
    ${SRCDIR}
    ${CMAKE_SOURCE_DIR}/externals/sdk
  )
  target_link_libraries(graphics_lua_api_test PRIVATE lua_static)
  if(MSVC)
    target_compile_options(graphics_lua_api_test PRIVATE /EHsc)
  endif()

  add_test(NAME graphics_lua_api COMMAND graphics_lua_api_test)
endif()
