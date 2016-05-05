cmake_minimum_required(VERSION 2.6)
project(chaos_driver_libera_cspi)

ADD_DEFINITIONS(-D_LIBERACSPI_)
INCLUDE_DIRECTORIES(../../ ../libera-driver-2-04-ebpp ../msp/src)
set (CMAKE_C_FLAGS "-std=gnu99 -DEBPP -DCORDIC_IGNORE_GAIN -D_REENTRANT -Idriver/libera-driver-2-04-ebpp -Imsp/src -I/cspi")

SET(LiberaCSPI_src  LiberaBrillianceCSPIDriver.cpp cordic.c pool.c cspi.c cspi_events.c ebpp.c)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
IF("${CMAKE_C_COMPILER}" MATCHES "arm-infn-linux-gnueabi-gcc") 
  MESSAGE(STATUS "Enabling compilation Libera CSPI")
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${LiberaCSPI_src})
  SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
  INSTALL_TARGETS(/lib ${PROJECT_NAME})
  FILE(GLOB ss cspi/*.h)
  INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera/driver/cspi)
  FILE(REMOVE UnitServerIgnore)
ELSE()
  FILE(WRITE UnitServerIgnore "disabled ${PROJECT_NAME}")
ENDIF()


   