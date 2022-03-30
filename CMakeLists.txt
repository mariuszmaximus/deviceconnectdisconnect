cmake_minimum_required(VERSION 3.0.0)
project(deviceconnectdisconnect VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(. SRC_FILES)

add_executable(deviceconnectdisconnect ${SRC_FILES})

if (WIN32)
    target_link_libraries(deviceconnectdisconnect pthread)
else()
    target_link_libraries(deviceconnectdisconnect udev pthread)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
