cmake_minimum_required(VERSION 2.8)
project(osm)

set(SUBHOOK_STATIC TRUE)
add_subdirectory(subhook)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
include_directories(subhook)

add_library(sdk STATIC
  sdk/amx/amx.h
  sdk/amx/sclinux.h
  sdk/amxplugin.cpp
  sdk/plugincommon.h
)

add_library(osm MODULE osm.cpp osm.def)
set_target_properties(osm PROPERTIES PREFIX "")

target_link_libraries(osm sdk subhook)
