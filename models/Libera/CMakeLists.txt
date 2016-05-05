cmake_minimum_required(VERSION 2.6)
project(chaos_driver_libera)
SET(DAQ_src LiberaData.cpp  CmdLiberaAcquire.cpp CmdLiberaDefault.cpp CmdLiberaEnv.cpp CmdLiberaTime.cpp SCLiberaCU.cpp  DafneAccumulatorBPMSync.cpp ChaosControllerLibera.cpp CmdDefaultDafneAccumulatorBPM.cpp CmdAcquireDafneAccumulatorBPM.cpp CmdEnvDafneAccumulatorBPM.cpp SCDafneAccumulatorBPMSync.cpp)
ADD_DEFINITIONS(-DEBPP)
SET(BasicDAQClient_src test/DAQClient.cpp)
ADD_SUBDIRECTORY(driver/cspi)
ADD_SUBDIRECTORY(driver/mci)
ADD_SUBDIRECTORY(driver/soft)


INCLUDE_DIRECTORIES( driver/cspi driver/mci driver/soft driver/libera-driver-2-04-ebpp/ .)
ADD_EXECUTABLE(BPMSync test/BPMSync.cpp)
ADD_EXECUTABLE(daqLiberaServer test/daqLiberaServer.cpp)
ADD_EXECUTABLE(daqLiberaClient test/daqLiberaClient.cpp ChaosControllerLibera.cpp)

IF(BUILD_FORCE_STATIC)

   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )

 #  set_target_properties(BPMSync PROPERTIES LINK_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY chaos_driver_libera ${DRIVERS_LIB})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_serial chaos_driver_misc common_misc_wavegenerators ${FrameworkLib})

TARGET_LINK_LIBRARIES(daqLiberaServer ${DAQ_LIBRARY} chaos_cutoolkit chaos_common  common_serial ${FrameworkLib})
TARGET_LINK_LIBRARIES(daqLiberaClient chaos_uitoolkit chaos_common chaos_driver_misc ${DAQ_LIBRARY} ${FrameworkLib})
TARGET_LINK_LIBRARIES(BPMSync ${DAQ_LIBRARY} chaos_driver_misc  chaos_uitoolkit  chaos_cutoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin daqLiberaServer)
INSTALL_TARGETS(/bin daqLiberaClient)
INSTALL_TARGETS(/bin BPMSync) 


INSTALL_TARGETS(/lib ${PROJECT_NAME})
FILE(GLOB ss *.h)
INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera)


   