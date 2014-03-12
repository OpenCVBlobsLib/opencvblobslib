file(GLOB cpp "*.cpp")
file(GLOB head "*.h")

source_group("Source" FILES ${cpp})
source_group("Headers" FILES ${head})

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PTHREADS_INCLUDE_DIR})

add_library(opencvblobslib STATIC ${cpp} ${head})

set_target_properties(opencvblobslib PROPERTIES
  PUBLIC_HEADER "${head}")
 
install(TARGETS opencvblobslib
  # IMPORTANT: Add the opencvblobslib library to the "export-set"
  EXPORT opencvblobslibTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/opencvblobslib"
    COMPONENT dev)

#set_property(TARGET opencvblobslib PROPERTY FOLDER "libraries")
# install (TARGETS opencvblobslib ARCHIVE DESTINATION /lib/opencvblobslib)
# install (FILES ${head} DESTINATION /include/opencvblobslib)
