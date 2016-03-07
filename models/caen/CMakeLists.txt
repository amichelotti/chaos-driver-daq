cmake_minimum_required(VERSION 2.6)
project(chaos_driver_daq_caen)
SET(DAQ_src RTCAEN.cpp RTCAEN775.cpp RTCAEN965.cpp RTCAEN792.cpp)
# ADD_DEFINITIONS(-DDEBUG)

IF(BUILD_FORCE_STATIC)
   SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY ${PROJECT_NAME})
ADD_EXECUTABLE(caenDAQ test/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common chaos_driver_misc common_vme_caen common_vme vme common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(caenDAQ ${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme_caen common_vme  vme common_debug ${FrameworkLib})

 INSTALL_TARGETS(/lib ${PROJECT_NAME})
 INSTALL_TARGETS(/bin caenDAQ)

 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/caen)

   

