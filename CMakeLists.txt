cmake_minimum_required(VERSION 2.6)
project(DAQ)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(DAQ_src models/Libera/LiberaBrillianceSlowDriver.cpp models/Libera/LiberaBrillianceCSPIDriver.cpp)
set (CMAKE_C_FLAGS "-std=gnu99 -DEBPP -DCORDIC_IGNORE_GAIN -D_REENTRANT -Imodels/Libera/driver/libera-driver-2-04-ebpp -Imodels/Libera/msp/src -Imodels/Libera/cspi")
SET(BasicDAQClient_src test/DAQClient.cpp)
INCLUDE_DIRECTORIES(models/Libera/cspi models/Libera/driver/libera-driver-2-04-ebpp models/Libera/msp/src)
ADD_DEFINITIONS(-DEBPP -DCORDIC_IGNORE_GAIN -D_REENTRANT)
SET(LiberaCSPI_src models/Libera/cspi/cordic.c models/Libera/cspi/pool.c models/Libera/cspi/cspi.c models/Libera/cspi/cspi_events.c models/Libera/cspi/ebpp.c)

 ADD_EXECUTABLE(daqServer test/daqServer.cpp)
 ADD_EXECUTABLE(daqClient test/daqClient.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(chaos_driver_DAQ STATIC ${DAQ_src} )
   ADD_LIBRARY(chaos_driver_libera STATIC ${LiberaCSPI_src})

ELSE()
    ADD_LIBRARY(chaos_driver_DAQ SHARED ${DAQ_src})	
   ADD_LIBRARY(chaos_driver_libera SHARED ${LiberaCSPI_src})
	 

ENDIF()

 TARGET_LINK_LIBRARIES(chaos_driver_DAQ chaos_cutoolkit chaos_common common_serial ${FrameworkLib})
 TARGET_LINK_LIBRARIES(daqServer chaos_driver_DAQ chaos_driver_libera chaos_cutoolkit chaos_common common_serial ${FrameworkLib})
 TARGET_LINK_LIBRARIES(daqClient chaos_uitoolkit chaos_common ${FrameworkLib})

 INSTALL_TARGETS(/bin daqServer)
 INSTALL_TARGETS(/bin daqClient)
 INSTALL_TARGETS(/lib chaos_driver_DAQ)
 INSTALL_TARGETS(/lib chaos_driver_libera)


