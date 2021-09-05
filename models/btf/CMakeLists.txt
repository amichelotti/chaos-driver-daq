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
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme_caen common_vme_sis common_vme common_debug ${FrameworkLib})
ADD_EXECUTABLE(daqbtfCU test/main.cpp)
TARGET_LINK_LIBRARIES(daqbtfCU ${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme_caen common_vme_sis common_vme common_debug ${FrameworkLib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_vme_caen common_vme_sis common_vme ${FrameworkLib})
 #INSTALL_TARGETS(/lib ${PROJECT_NAME})
 INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
 #INSTALL_TARGETS(/bin daqbtfCU)
 INSTALL(TARGETS daqbtfCU
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/btf)

   

