cmake_minimum_required(VERSION 2.6)
project(chaos_driver_libera_mci)
IF("${CMAKE_C_COMPILER}" MATCHES "i686-nptl-linux-gnu-gcc")
  MESSAGE(STATUS "Enabling compilation MCI")
  ADD_DEFINITIONS(-D_LIBERAMCI_ -std=c++11)
  LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)
  INCLUDE_DIRECTORIES(../../ ../libera-driver-2-04-ebpp ../cspi ../msp/src ../mci/include include/omniORB4 /usr/include)		
  ADD_LIBRARY(${PROJECT_NAME} STATIC LiberaBrilliancePlusDriver.cpp)
  SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} liberaistd2.8 liberamci2.8 liberaisig2.8 liberainet2.8 omniORB4 omniDynamic4 omnithread PARENT_SCOPE)
  INSTALL_TARGETS(/lib ${PROJECT_NAME} )
    FILE(REMOVE UnitServerIgnore)
ELSE()
  FILE(WRITE UnitServerIgnore "disabled ${PROJECT_NAME}")
ENDIF()


   