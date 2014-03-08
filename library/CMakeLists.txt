file(GLOB cpp "*.cpp")
file(GLOB head "*.h")

source_group("Source" FILES ${cpp})
source_group("Headers" FILES ${head})

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PTHREADS_INCLUDE_DIR})

add_library(opencvblobslib STATIC ${cpp} ${head})
#set_property(TARGET opencvblobslib PROPERTY FOLDER "libraries")
install (TARGETS opencvblobslib ARCHIVE DESTINATION /lib/opencvblobslib)
install (FILES ${head} DESTINATION /include/opencvblobslib)
