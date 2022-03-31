include(CTest)
enable_testing()

aux_source_directory(. SRC_FILES)

add_executable(sample_device_connect_disconnect ${SRC_FILES})

if (WIN32)
    target_link_libraries(sample_device_connect_disconnect deviceconnectdisconnect) 

    # set macro for replace device class
    # FTDI:        
    # target_compile_definitions(deviceconnectdisconnect PRIVATE REPLACE_GUID_HARDWARE="{d1e8fe6a-ab75-4d9e-97d2-06fa22c7736c}")
    # USB_MEMORY:  
    # target_compile_definitions(deviceconnectdisconnect PRIVATE REPLACE_GUID_HARDWARE="{36fc9e60-c465-11cf-8056-444553540000}")
else()
    target_link_libraries(sample_device_connect_disconnect deviceconnectdisconnect)
endif()



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
