cmake_minimum_required(VERSION 2.6)
project(chaos_driver_daq_btf)
SET(DAQ_src RTBTFdaqCU.cpp RTBTFdaqSimCU.cpp )
ADD_DEFINITIONS(-fpermissive -DDEBUG)
IF(BUILD_FORCE_STATIC)
   SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY ${PROJECT_NAME})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme vme common_debug ${FrameworkLib})
ADD_EXECUTABLE(daqbtfCU test/main.cpp)
TARGET_LINK_LIBRARIES(daqbtfCU ${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme chaos_driver_misc vme chaos_data_import common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme vme ${FrameworkLib})
 INSTALL_TARGETS(/lib ${PROJECT_NAME})
 INSTALL_TARGETS(/bin daqbtfCU)

 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/btf)

   

