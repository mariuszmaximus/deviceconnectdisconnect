cmake_minimum_required(VERSION 3.1)

##
## PROJECT
## name and version
##
project(deviceconnectdisconnect VERSION 1.0.0 LANGUAGES CXX)

##
## MAIN_PROJECT CHECK
## determine if deviceconnectdisconnect is built as a subproject (using add_subdirectory) or if it is the main project
##
set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

##
## INCLUDE
##
##
add_subdirectory(src)

option(BuildSampleApp     "Build sample APP." ON)

if (BuildSampleApp)
    add_subdirectory(example)
endif()


