file(GLOB cpp "*.cpp")
file(GLOB head "*.h")
file(GLOB img "*.png")

source_group("Source" FILES ${cpp})
source_group("Headers" FILES ${head})

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PTHREADS_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/library)

add_executable(opencvblobslibExample ${cpp} ${head} ${img})
target_link_libraries(opencvblobslibExample opencvblobslib)
target_link_libraries(opencvblobslibExample ${OpenCV_LIBS})

if(WIN32)
	if(NOT ${PTHREADS_LIB_DIR} STREQUAL "")
		target_link_libraries(opencvblobslibExample ${PTHREADS_LIB_DIR}/pthreadVC2.lib)
		message("Linked " ${PTHREADS_LIB_DIR}/pthreadVC2.lib)
	endif()
elseif(UNIX)
	target_link_libraries(opencvblobslibExample ${CMAKE_THREAD_LIBS_INIT})
	message("Linked " ${CMAKE_THREAD_LIBS_INIT})
endif()

#I copy the images needed for the example to run
file(COPY ${img} DESTINATION ${CMAKE_BINARY_DIR}/example)

install(TARGETS opencvblobslibExample
  # IMPORTANT: Add the opencvblobslibExample executable to the "export-set"
  EXPORT opencvblobslibTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
