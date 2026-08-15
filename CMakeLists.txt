cmake_minimum_required(VERSION 3.21)

project(OpenVSM)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/model/VERSION.txt" OPENVSM_VERSION)
option(DLL_WITH_INSTALLER "Enable the Inno Setup installer target" OFF)
option(BUILD_DOCS "Build the Doxygen developer reference" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")  
  message(INFO "MSVC found, continue...")
else()
  message(FATAL_ERROR "Unsupported compiler, only MSVC is supported.")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

function(openvsm_add_bundled_lua)
  # The pinned Lua project declares CMake 3.1 compatibility. CMake 4 rejects
  # policy versions below 3.5 and deprecates versions below 3.10. Raise only
  # the dependency's effective policy minimum and leave the submodule unmodified.
  if(NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
  elseif(CMAKE_POLICY_VERSION_MINIMUM VERSION_LESS 3.10)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
  endif()

  add_subdirectory(externals/Lua)
endfunction()

openvsm_add_bundled_lua()
if(BUILD_TESTS)
  enable_testing()
endif()

add_subdirectory(model/cpp/log)
add_subdirectory(model)
add_subdirectory(model/lua)

if(BUILD_DOCS)
  add_subdirectory(docs)
endif()


if(DLL_WITH_INSTALLER)
  find_program(INNO_SETUP_COMPILER NAMES ISCC.exe ISCC REQUIRED)
  set(INNO_SETUP_SCRIPT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/installer/openvsm.iss")
  set(INNO_SETUP_OUTPUT_DIR "${CMAKE_BINARY_DIR}/installer")
  add_custom_target(BuildInstaller
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${INNO_SETUP_OUTPUT_DIR}"
                    COMMAND "${INNO_SETUP_COMPILER}"
                            "/DOpenVsmVersion=${OPENVSM_VERSION}"
                            "/DOpenVsmDllPath=$<TARGET_FILE:VSM_DLL>"
                            "/DOpenVsmLoggerPath=$<TARGET_FILE:tinyLog>"
                            "/DOpenVsmOutputDir=${INNO_SETUP_OUTPUT_DIR}"
                            "${INNO_SETUP_SCRIPT_PATH}"
                    COMMENT "Building the OpenVSM installer"
                    VERBATIM
  )
  add_dependencies(BuildInstaller VSM_DLL tinyLog)
endif()

target_include_directories(VSM_DLL PUBLIC externals)
target_include_directories(VSM_DLL PUBLIC model/cpp)
target_include_directories(VSM_DLL PUBLIC externals/sdk)


