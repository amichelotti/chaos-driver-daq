cmake_minimum_required(VERSION 2.8)
project(chaos_driver_libera)
SET(DAQ_src LiberaData.cpp  CmdLiberaAcquire.cpp CmdLiberaDefault.cpp CmdLiberaEnv.cpp CmdLiberaTime.cpp SCLiberaCU.cpp  DafneAccumulatorBPMSync.cpp ChaosControllerLibera.cpp CmdDefaultDafneAccumulatorBPM.cpp CmdAcquireDafneAccumulatorBPM.cpp CmdEnvDafneAccumulatorBPM.cpp SCDafneAccumulatorBPMSync.cpp)
ADD_DEFINITIONS(-DEBPP)
SET(BasicDAQClient_src test/DAQClient.cpp)
ADD_SUBDIRECTORY(driver/cspi)
ADD_SUBDIRECTORY(driver/mci)
ADD_SUBDIRECTORY(driver/soft)

ADD_DEFINITIONS(${DRIVERS_FLAGS})

INCLUDE_DIRECTORIES( driver/cspi driver/mci driver/mci/include driver/soft driver/libera-driver-2-04-ebpp/ . ../ ../../ driver/mci/include/omniORB4)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/driver/mci/lib)
ADD_EXECUTABLE(BPMSync test/BPMSync.cpp)
ADD_EXECUTABLE(daqLiberaServer test/daqLiberaServer.cpp)
ADD_EXECUTABLE(daqLiberaClient test/daqLiberaClient.cpp ChaosControllerLibera.cpp)

IF(BUILD_FORCE_STATIC)

   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )

   #set_target_properties(BPMSync PROPERTIES LINK_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   set_target_properties(daqLiberaServer PROPERTIES LINK_FLAGS "-Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY ${PROJECT_NAME} ${DRIVERS_LIB})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_serial chaos_driver_misc common_misc_wavegenerators ${FrameworkLib})

TARGET_LINK_LIBRARIES(daqLiberaServer ${DAQ_LIBRARY} chaos_driver_misc chaos_cutoolkit chaos_common  common_serial chaos_driver_misc ${FrameworkLib})
TARGET_LINK_LIBRARIES(daqLiberaClient chaos_uitoolkit chaos_driver_misc chaos_common chaos_driver_misc ${DAQ_LIBRARY} ${FrameworkLib})
TARGET_LINK_LIBRARIES(BPMSync ${PROJECT_NAME} chaos_driver_misc  chaos_uitoolkit  chaos_cutoolkit chaos_common ${FrameworkLib})


INSTALL_TARGETS(/bin daqLiberaServer)
INSTALL_TARGETS(/bin daqLiberaClient)
INSTALL_TARGETS(/bin BPMSync) 


INSTALL_TARGETS(/lib ${PROJECT_NAME})
InstallHeaders(.)
InstallHeaders(driver/cspi)
#FILE(GLOB ss *.h)
#INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera)


   
