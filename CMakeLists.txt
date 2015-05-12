cmake_minimum_required(VERSION 2.6)
project(DAQ)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(DAQ_src models/Libera/LiberaBrillianceSlowDriver.cpp)

SET(BasicDAQClient_src test/DAQClient.cpp)

 ADD_EXECUTABLE(daqServer test/daqServer.cpp)
 ADD_EXECUTABLE(daqClient test/daqClient.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(chaos_driver_DAQ STATIC ${DAQ_src})		 

ELSE()
    ADD_LIBRARY(chaos_driver_DAQ SHARED ${DAQ_src})		 

ENDIF()

 TARGET_LINK_LIBRARIES(chaos_driver_DAQ chaos_cutoolkit chaos_common common_serial ${FrameworkLib})
 TARGET_LINK_LIBRARIES(daqServer chaos_driver_DAQ chaos_cutoolkit chaos_common common_serial ${FrameworkLib})
 TARGET_LINK_LIBRARIES(daqClient chaos_uitoolkit chaos_common ${FrameworkLib})

 INSTALL_TARGETS(/bin daqServer)
 INSTALL_TARGETS(/bin daqClient)
 INSTALL_TARGETS(/lib chaos_driver_DAQ)

