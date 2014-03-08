cmake_minimum_required ( VERSION 2.6)
project (Opencvblobslib)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
#message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_DEBUG_POSTFIX "_d")


cmake_minimum_required(VERSION 2.8)
find_package( OpenCV REQUIRED )

#set(OPENCV_INCLUDE_DIR "" CACHE PATH "Opencv Include Directory")
#set(OPENCV_LIB_DIR "" CACHE PATH "Opencv Lib Directory")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PTHREADS_INCLUDE_DIR "" CACHE PATH "Pthreads Include Directory")
	set(PTHREADS_BIN_DIR "" CACHE PATH "Pthreads Bin Directory")
	set(slib "lib")
    set(OS "Windows")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(slib "a")
    set(OS "Linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


add_subdirectory(library)
add_subdirectory(example)
