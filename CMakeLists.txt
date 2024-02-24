cmake_minimum_required(VERSION 3.21)

project(OpenVSM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_MAKE_PROGRAM make)
SET(CMAKE_USE_RELATIVE_PATHS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(BasicSettingsFile ${CMAKE_CURRENT_SOURCE_DIR}/wakadll/cpp/settings/basic_settings.yml)

#Build Lua modules
#add_subdirectory(wakadll/lua)
add_subdirectory(externals/Lua)
add_subdirectory(model)
add_subdirectory(model/cpp/log)

if(DLL_WITH_INSTALLER)  
  set(INNO_SETUP_SCRIPT_PATH "installer/waka.iss")
  add_custom_target(BuildInstaller 
                    COMMAND ISCC.exe ${INNO_SETUP_SCRIPT_PATH}
                    COMMENT "Building the installer using Inno Setup..."
  )
  add_dependencies(BuildInstaller VSM_DLL)
endif()

#add_custom_target(BasicSettings ALL DEPENDS ${BasicSettingsFile})
#get_filename_component(BasicSettingsFileName ${BasicSettingsFile} NAME_WE)
#add_custom_command(TARGET BasicSettings        
#        COMMENT "Copying settings file ${BasicSettingsFile}"
#        COMMAND ${CMAKE_COMMAND} -E copy ${BasicSettingsFile} "${CMAKE_BINARY_DIR}/${BasicSettingsFileName}.yml"
#)

target_include_directories(VSM_DLL PUBLIC externals)
target_include_directories(VSM_DLL PUBLIC model/cpp)
target_include_directories(VSM_DLL PUBLIC externals/sdk)


