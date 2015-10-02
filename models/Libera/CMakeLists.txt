cmake_minimum_required(VERSION 2.6)

SET(DAQ_src LiberaData.cpp LiberaBrillianceCSPIDriver.cpp CmdLiberaAcquire.cpp CmdLiberaDefault.cpp CmdLiberaEnv.cpp CmdLiberaTime.cpp SCLiberaCU.cpp  DafneAccumulatorBPMSync.cpp )
set (CMAKE_C_FLAGS "-std=gnu99 -DEBPP -DCORDIC_IGNORE_GAIN -D_REENTRANT -Idriver/libera-driver-2-04-ebpp -Imsp/src -I/cspi")
SET(BasicDAQClient_src test/DAQClient.cpp)
INCLUDE_DIRECTORIES(. cspi driver/libera-driver-2-04-ebpp msp/src)
ADD_DEFINITIONS(-DEBPP -DCSPI -DCORDIC_IGNORE_GAIN -D_REENTRANT)
SET(LiberaCSPI_src cspi/cordic.c cspi/pool.c cspi/cspi.c cspi/cspi_events.c cspi/ebpp.c)


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   ADD_LIBRARY(chaos_driver_libera STATIC ${DAQ_src} )
   ADD_LIBRARY(chaos_driver_libera_cspi STATIC ${LiberaCSPI_src})

ELSE()
  ADD_LIBRARY(chaos_driver_libera SHARED ${DAQ_src})	
  ADD_LIBRARY(chaos_driver_libera_cspi SHARED ${LiberaCSPI_src})
	 
ENDIF()

SET(DAQ_LIBRARY chaos_driver_libera chaos_driver_libera_cspi)
TARGET_LINK_LIBRARIES(chaos_driver_libera chaos_cutoolkit chaos_common common_serial chaos_driver_misc ${FrameworkLib})

ADD_EXECUTABLE(BPMSync test/BPMSync.cpp)
ADD_EXECUTABLE(daqLiberaServer test/daqLiberaServer.cpp)
ADD_EXECUTABLE(daqLiberaClient test/daqLiberaClient.cpp ChaosControllerLibera.cpp)

TARGET_LINK_LIBRARIES(daqLiberaServer ${DAQ_LIBRARY} chaos_cutoolkit chaos_common  common_serial ${FrameworkLib})
TARGET_LINK_LIBRARIES(daqLiberaClient chaos_uitoolkit chaos_common chaos_driver_misc ${DAQ_LIBRARY} ${FrameworkLib})
TARGET_LINK_LIBRARIES(BPMSync chaos_uitoolkit  chaos_cutoolkit chaos_common chaos_driver_misc ${DAQ_LIBRARY} ${FrameworkLib})

INSTALL_TARGETS(/bin daqLiberaServer)
INSTALL_TARGETS(/bin daqLiberaClient)
INSTALL_TARGETS(/bin BPMSync) 

 INSTALL_TARGETS(/lib chaos_driver_libera_cspi)
 INSTALL_TARGETS(/lib chaos_driver_libera)
 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera)
 FILE(GLOB ss cspi/*.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera/cspi)
   

