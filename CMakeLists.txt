cmake_minimum_required(VERSION 2.6)
project(daq)
IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  message(STATUS "Including Libera")
  ADD_SUBDIRECTORY(models/Libera)
  ADD_DEFINITIONS(-DLIBERA)
ENDIF()
  


