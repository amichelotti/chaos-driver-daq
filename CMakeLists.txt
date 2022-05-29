cmake_minimum_required(VERSION 2.9)
project(daq)
find_package(chaos MODULE REQUIRED)
include_directories(../../ ${chaos_INCLUDE_DIRS})

ADD_SUBDIRECTORY(models)


# install(DIRECTORY DESTINATION include/driver/daq)  


