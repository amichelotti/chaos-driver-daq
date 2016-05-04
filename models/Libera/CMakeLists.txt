cmake_minimum_required(VERSION 2.6)

SET(DAQ_src LiberaData.cpp  LiberaSoftDriver.cpp CmdLiberaAcquire.cpp CmdLiberaDefault.cpp CmdLiberaEnv.cpp CmdLiberaTime.cpp SCLiberaCU.cpp  DafneAccumulatorBPMSync.cpp ChaosControllerLibera.cpp CmdDefaultDafneAccumulatorBPM.cpp CmdAcquireDafneAccumulatorBPM.cpp CmdEnvDafneAccumulatorBPM.cpp SCDafneAccumulatorBPMSync.cpp)

SET(BasicDAQClient_src test/DAQClient.cpp)
  ADD_DEFINITIONS(-DEBPP -DCSPI -DCORDIC_IGNORE_GAIN -D_REENTRANT)
  INCLUDE_DIRECTORIES(. cspi driver/libera-driver-2-04-ebpp msp/src driver/misc)



IF("${CMAKE_C_COMPILER}" MATCHES "arm-infn-linux-gnueabi-gcc") 
  MESSAGE(STATUS "Enabling compilation Libera CSPI")
  ADD_DEFINITIONS(-D_LIBERACSPI_)
  set (CMAKE_C_FLAGS "-std=gnu99 -DEBPP -DCORDIC_IGNORE_GAIN -D_REENTRANT -Idriver/libera-driver-2-04-ebpp -Imsp/src -I/cspi")

  SET(LiberaCSPI_src  LiberaBrillianceCSPIDriver.cpp cspi/cordic.c cspi/pool.c cspi/cspi.c cspi/cspi_events.c cspi/ebpp.c)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  ADD_LIBRARY(chaos_driver_libera_cspi STATIC ${LiberaCSPI_src})
  SET(DAQ_LIBRARY chaos_driver_libera_cspi)
   INSTALL_TARGETS(/lib chaos_driver_libera_cspi)
 FILE(GLOB ss cspi/*.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera/cspi)
ELSE()
  MESSAGE(STATUS "Disabling compilation of libera CSPI")
ENDIF()

IF("${CMAKE_C_COMPILER}" MATCHES "i686-nptl-linux-gnu-gcc")
#FIND_PATH(CORBA_INC CORBA.h)
#IF(NOT CORBA_INC)
#MESSAGE(FATAL_ERROR "You should include CORBA development try package: libomniorb4-dev")
#ENDIF()

	ADD_DEFINITIONS(-D_LIBERAMCI_ -std=c++11)
	INCLUDE_DIRECTORIES(. driver/liberaPlus/include)
	
  ADD_LIBRARY(chaos_driver_libera_mci STATIC LiberaBrilliancePlusDriver.cpp)
  SET(DAQ_LIBRARY chaos_driver_libera_mci liberaistd2.8 liberamci2.8 liberaisig2.8 liberainet2.8 omniORB4 omniDynamic4 omnithread)
  
  INSTALL_TARGETS(/lib chaos_driver_libera_mci )

  MESSAGE(STATUS "Enabling compilation LiberaPlus")
ELSE()
  MESSAGE(STATUS "Disabling compilation of LiberaPlus")
ENDIF()






ADD_EXECUTABLE(BPMSync test/BPMSync.cpp)
ADD_EXECUTABLE(daqLiberaServer test/daqLiberaServer.cpp)
ADD_EXECUTABLE(daqLiberaClient test/daqLiberaClient.cpp ChaosControllerLibera.cpp)

IF(BUILD_FORCE_STATIC)

   ADD_LIBRARY(chaos_driver_libera STATIC ${DAQ_src} )

 #  set_target_properties(BPMSync PROPERTIES LINK_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
  ADD_LIBRARY(chaos_driver_libera SHARED ${DAQ_src})	
	 
ENDIF()

SET(DAQ_LIBRARY chaos_driver_libera ${DAQ_LIBRARY})
TARGET_LINK_LIBRARIES(chaos_driver_libera chaos_cutoolkit chaos_common common_serial chaos_driver_misc common_misc_wavegenerators ${FrameworkLib})

TARGET_LINK_LIBRARIES(daqLiberaServer ${DAQ_LIBRARY} chaos_cutoolkit chaos_common  common_serial ${FrameworkLib})
TARGET_LINK_LIBRARIES(daqLiberaClient chaos_uitoolkit chaos_common chaos_driver_misc ${DAQ_LIBRARY} ${FrameworkLib})
TARGET_LINK_LIBRARIES(BPMSync ${DAQ_LIBRARY} chaos_driver_misc  chaos_uitoolkit  chaos_cutoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin daqLiberaServer)
INSTALL_TARGETS(/bin daqLiberaClient)
INSTALL_TARGETS(/bin BPMSync) 


 INSTALL_TARGETS(/lib chaos_driver_libera)
 FILE(GLOB ss *.h)
 INSTALL(FILES ${ss} DESTINATION include/driver/daq/models/Libera)


   