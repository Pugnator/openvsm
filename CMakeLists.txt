cmake_minimum_required(VERSION 3.21)

project(OpenVSM)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/model/VERSION.txt" OPENVSM_VERSION)
option(DLL_WITH_INSTALLER "Enable the Inno Setup installer target" OFF)
option(BUILD_DOCS "Build the Doxygen developer reference" OFF)
set(OPENVSM_SIGN_CERTIFICATE_SHA1 "" CACHE STRING "SHA-1 thumbprint of the release-signing certificate")
set(OPENVSM_SIGN_TIMESTAMP_URL "https://timestamp.digicert.com" CACHE STRING "RFC 3161 timestamp service")
set(OPENVSM_SIGNTOOL "" CACHE FILEPATH "Path to signtool.exe")

if(OPENVSM_SIGN_CERTIFICATE_SHA1)
  string(REPLACE " " "" OPENVSM_SIGN_CERTIFICATE_SHA1 "${OPENVSM_SIGN_CERTIFICATE_SHA1}")

  if(NOT OPENVSM_SIGNTOOL)
    if(WIN32)
      file(GLOB OPENVSM_SIGNTOOL_CANDIDATES
        "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x64/signtool.exe"
        "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x86/signtool.exe"
      )
      if(OPENVSM_SIGNTOOL_CANDIDATES)
        list(SORT OPENVSM_SIGNTOOL_CANDIDATES COMPARE NATURAL ORDER DESCENDING)
        list(GET OPENVSM_SIGNTOOL_CANDIDATES 0 OPENVSM_SIGNTOOL)
      endif()
    else()
      find_program(OPENVSM_SIGNTOOL NAMES signtool.exe signtool)
    endif()
  endif()

  if(NOT EXISTS "${OPENVSM_SIGNTOOL}")
    message(FATAL_ERROR "Code signing was requested, but signtool.exe was not found. Set OPENVSM_SIGNTOOL.")
  endif()

  set(OPENVSM_SIGN_ARGS
    sign
    /sha1 "${OPENVSM_SIGN_CERTIFICATE_SHA1}"
    /fd SHA256
    /tr "${OPENVSM_SIGN_TIMESTAMP_URL}"
    /td SHA256
  )
endif()

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

  # Lua's subproject registers an interpreter test even though its interpreter
  # target is disabled when embedded. OpenVSM tests the linked Lua library
  # through its own native and script tests instead.
  set(LUA_ENABLE_TESTING OFF CACHE BOOL "Build the bundled Lua tests" FORCE)

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
  set(OPENVSM_INSTALLER_PATH "${INNO_SETUP_OUTPUT_DIR}/openvsm-${OPENVSM_VERSION}-setup.exe")
  set(OPENVSM_INSTALLER_SIGN_COMMAND)
  if(OPENVSM_SIGN_CERTIFICATE_SHA1)
    list(APPEND OPENVSM_INSTALLER_SIGN_COMMAND
      COMMAND "${OPENVSM_SIGNTOOL}" ${OPENVSM_SIGN_ARGS} "${OPENVSM_INSTALLER_PATH}"
    )
  endif()
  add_custom_target(BuildInstaller
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${INNO_SETUP_OUTPUT_DIR}"
                    COMMAND "${INNO_SETUP_COMPILER}"
                            "/DOpenVsmVersion=${OPENVSM_VERSION}"
                            "/DOpenVsmDllPath=$<TARGET_FILE:VSM_DLL>"
                            "/DOpenVsmOutputDir=${INNO_SETUP_OUTPUT_DIR}"
                            "${INNO_SETUP_SCRIPT_PATH}"
                    ${OPENVSM_INSTALLER_SIGN_COMMAND}
                    COMMENT "Building the OpenVSM installer"
                    VERBATIM
  )
  add_dependencies(BuildInstaller VSM_DLL)
endif()

target_include_directories(VSM_DLL PUBLIC externals)
target_include_directories(VSM_DLL PUBLIC model/cpp)
target_include_directories(VSM_DLL PUBLIC externals/sdk)


