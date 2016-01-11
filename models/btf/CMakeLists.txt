cmake_minimum_required(VERSION 2.6)
project(chaos_driver_daq_btf)
SET(DAQ_src RTBTFdaqCU.cpp)
ADD_DEFINITIONS(-fpermissive)
IF(BUILD_FORCE_STATIC)
   SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY ${PROJECT_NAME})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme vme ${FrameworkLib})

 INSTALL_TARGETS(/lib ${PROJECT_NAME})
 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/btf)

   

