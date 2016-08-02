cmake_minimum_required(VERSION 2.8)
project(daq)
IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  message(STATUS "Including Libera")
  SET(CHAOS_INSTALL_DIRS models/Libera models/Libera/cspi)
  ADD_DEFINITIONS(-DLIBERA)
  ADD_SUBDIRECTORY(Libera)
  ADD_SUBDIRECTORY(btf)
  ADD_SUBDIRECTORY(caen)

ENDIF()


# install(DIRECTORY DESTINATION include/driver/daq)  


