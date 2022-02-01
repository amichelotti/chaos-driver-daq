cmake_minimum_required(VERSION 2.8)
project(chaos_driver_libera)
## dependency on chaoscontroller
# DafneAccumulatorBPMSync.cpp CmdDefaultDafneAccumulatorBPM.cpp CmdAcquireDafneAccumulatorBPM.cpp CmdEnvDafneAccumulatorBPM.cpp SCDafneAccumulatorBPMSync.cpp  ChaosControllerLibera.cpp)
SET(DAQ_src LiberaData.cpp  CmdLiberaAcquire.cpp CmdLiberaDefault.cpp CmdLiberaEnv.cpp CmdLiberaTime.cpp SCLiberaCU.cpp driver/soft/LiberaSoftDriver.cpp)
SET(ADD_LIBS )

IF (CHAOS_EPICS_SUPPORT)
SET(DAQ_src ${DAQ_src} driver/epics/LiberaEpicsBase.cpp driver/epics/LiberaEpicsDriver.cpp driver/epics/LiberaBrillianceEpicsDriver.cpp)
SET(ADD_LIBS chaos_driver_epics)
ENDIF()
ADD_DEFINITIONS(-DEBPP)
SET(BasicDAQClient_src test/DAQClient.cpp)
ADD_SUBDIRECTORY(driver/cspi)
ADD_SUBDIRECTORY(driver/mci)
# ADD_SUBDIRECTORY(driver/soft)

ADD_DEFINITIONS(${DRIVERS_FLAGS})

INCLUDE_DIRECTORIES( driver/cspi driver/mci driver/mci/include driver/soft driver/libera-driver-2-04-ebpp/ . ../ ../../ driver/mci/include/omniORB4)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/driver/mci/lib)
# ADD_EXECUTABLE(BPMSync test/BPMSync.cpp)
ADD_EXECUTABLE(daqLiberaServer test/daqLiberaServer.cpp )



# ADD_EXECUTABLE(daqLiberaClient test/daqLiberaClient.cpp ChaosControllerLibera.cpp)

IF(BUILD_FORCE_STATIC)

   ADD_LIBRARY(${PROJECT_NAME} STATIC ${DAQ_src} )

   #set_target_properties(BPMSync PROPERTIES LINK_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
   set_target_properties(daqLiberaServer PROPERTIES LINK_FLAGS "-u ZMQDirectIOServerObjectFactoryInstancer -u ZMQDirectIOClientObjectFactoryInstancer -u DefaultCommandDispatcherObjectFactoryInstancer -u ZMQClientObjectFactoryInstancer -u ZMQServerObjectFactoryInstancer -u SQLiteEntityDBObjectFactoryInstancer -u SharedCommandDispatcherObjectFactoryInstancer -u IODirectIOPSMsgDriverObjectFactoryInstancer")
#  
#   set_target_properties(daqLiberaServer PROPERTIES LINK_FLAGS "-Wl,-lchaos_common -Wl,--no-whole-archive")
ELSE()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY ${PROJECT_NAME} ${DRIVERS_LIB})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_cutoolkit chaos_common common_serial common_misc_wavegenerators ${FrameworkLib})
TARGET_LINK_LIBRARIES(daqLiberaServer ${DAQ_LIBRARY}  chaos_cutoolkit chaos_common  common_serial ${ADD_LIBS} ${FrameworkLib})

# TARGET_LINK_LIBRARIES(daqLiberaClient  chaos_common chaos_driver_misc chaos_metadata_service_client ${DAQ_LIBRARY} ${FrameworkLib} )
# TARGET_LINK_LIBRARIES(BPMSync ${PROJECT_NAME}  chaos_cutoolkit chaos_common ${FrameworkLib})


#INSTALL_TARGETS(/bin daqLiberaServer)
INSTALL(TARGETS daqLiberaServer
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
 #INSTALL_TARGETS(/bin daqLiberaClient)
#  INSTALL(TARGETS daqLiberaClient
#         DESTINATION "bin"
#         PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
# #INSTALL_TARGETS(/bin BPMSync) 
# INSTALL(TARGETS BPMSync
#         DESTINATION "bin"
#         PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

#INSTALL_TARGETS(/lib ${PROJECT_NAME})
INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/driver/daq/models/Libera)
FILE(GLOB files "driver/cspi/*.h")
INSTALL(FILES ${files} DESTINATION include/driver/daq/models/Libera/driver/cspi)


#FILE(GLOB ss *.h)
#INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera)


   
