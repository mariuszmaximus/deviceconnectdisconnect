cmake_minimum_required(VERSION 3.0)
project(ProjectName)

set(LIB_NAME deviceconnectdisconnect)

aux_source_directory(. SRC_FILES)

add_library(${LIB_NAME} STATIC ${SRC_FILES})

target_include_directories(${LIB_NAME} INTERFACE .)

if (WIN32)
    target_link_libraries(${LIB_NAME} pthread ) 
else()
    target_link_libraries(${LIB_NAME} udev pthread )
endif()


target_compile_options(
        ${LIB_NAME} PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wall -O3 -fPIC>
                    PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall -O3 -fPIC>
                    PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>: /O2>
)

