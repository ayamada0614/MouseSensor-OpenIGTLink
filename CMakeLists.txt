PROJECT(MouseSensor)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

# OpenIGTLink stuff
FIND_PACKAGE(OpenIGTLink REQUIRED)

# Include dirs
include(${OpenIGTLink_USE_FILE})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${OpenIGTLink_INCLUDE_DIRS}
 )

file(GLOB headers 
 "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
 )

# --------------------------------------------------------------------------
# Sources
set(Receiver_SOURCES
${Receiver_SOURCES}
 MouseSensorOpenIGTLink.cxx
)
set(lib_name MouseSensorOpenIGTLink)

# --------------------------------------------------------------------------
# Build and install the library

ADD_LIBRARY(${lib_name}
 ${Receiver_SOURCES}
 )

ADD_EXECUTABLE(${lib_name}  
 ${Receiver_SOURCES}
 )

target_link_libraries(${lib_name}
 ${LINK_LIBS}
 OpenIGTLink
 )
