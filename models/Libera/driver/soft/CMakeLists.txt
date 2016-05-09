cmake_minimum_required(VERSION 2.6)
project(chaos_driver_libera_soft)

MESSAGE(STATUS "Enabling compilation Libera Soft")
ADD_DEFINITIONS(-D_LIBERASOFT_)
INCLUDE_DIRECTORIES(../../ ../cspi ../libera-driver-2-04-ebpp)	
ADD_LIBRARY(${PROJECT_NAME} STATIC LiberaSoftDriver.cpp)
SET(DRIVERS_LIB ${DRIVERS_LIB} ${PROJECT_NAME} PARENT_SCOPE)
INSTALL_TARGETS(/lib ${PROJECT_NAME}) 