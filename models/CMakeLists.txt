cmake_minimum_required(VERSION 2.9)
project(daq)
IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  message(STATUS "Including Libera")
  SET(CHAOS_INSTALL_DIRS models/Libera models/Libera/cspi)
  ADD_DEFINITIONS(-DLIBERA)
  ADD_SUBDIRECTORY(Libera)
  IF(CHAOS_VME)
    FILE(REMOVE btf/UnitServerIgnore)
    FILE(REMOVE caen/UnitServerIgnore)
      ADD_SUBDIRECTORY(btf)
      ADD_SUBDIRECTORY(caen)
    ELSE()
      FILE(WRITE caen/UnitServerIgnore "disabled ${PROJECT_NAME}")
      FILE(WRITE btf/UnitServerIgnore "disabled ${PROJECT_NAME}")
    ENDIF()
ENDIF()


# install(DIRECTORY DESTINATION include/driver/daq)  


