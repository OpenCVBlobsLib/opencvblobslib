file(GLOB cpp "*.cpp")
file(GLOB head "*.h")
file(GLOB img "*.png")

if(${OS} MATCHES "Windows")
	if(NOT ${PTHREADS_BIN_DIR} STREQUAL "")
		file(GLOB_RECURSE pthreads_lib ${PTHREADS_BIN_DIR}/*.${slib})
		foreach (lib ${pthreads_lib})
		list(APPEND pthreads_libs_app general ${lib})
	endforeach(lib)
	endif()
endif()

source_group("Source" FILES ${cpp})
source_group("Headers" FILES ${head})

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PTHREADS_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/library)

add_executable(opencvblobslibExample ${cpp} ${head} ${img})
target_link_libraries(opencvblobslibExample opencvblobslib)
target_link_libraries(opencvblobslibExample ${OpenCV_LIBS})

if(${OS} MATCHES "Windows")
	if(NOT ${PTHREADS_BIN_DIR} STREQUAL "")
		target_link_libraries(opencvblobslibExample ${pthreads_libs_app})
		#message("asd:" ${CMAKE_BINARY_DIR})
	endif()
endif()

#I copy the images needed for the example to run
file(COPY ${img} DESTINATION ${CMAKE_BINARY_DIR}/example)
