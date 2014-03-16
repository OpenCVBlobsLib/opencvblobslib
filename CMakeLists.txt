cmake_minimum_required ( VERSION 2.6)
project (opencvblobslib)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(OPENCVBLOBSLIB_VERSION 1.0.1)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/opencvblobslib)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
 
# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
  
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_DEBUG_POSTFIX "_d")



find_package( OpenCV REQUIRED )

IF(WIN32)
    set(PTHREADS_INCLUDE_DIR "" CACHE PATH "Pthreads Include Directory")
	set(PTHREADS_LIB_DIR "" CACHE PATH "Pthreads Lib Directory")
ELSEIF(UNIX)
	find_package(Threads)
ENDIF()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_subdirectory(library)
add_subdirectory(example)

# The interesting stuff goes here
# ===============================
 
# Add all targets to the build-tree export set
export(TARGETS opencvblobslib opencvblobslibExample
  FILE "${PROJECT_BINARY_DIR}/opencvblobslibTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE opencvblobslib)

# Create the opencvblobslibConfig.cmake and opencvblobslibConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}/opencvblobslib")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/library")
configure_file(opencvblobslibConfig.cmake.in
  "${PROJECT_BINARY_DIR}/opencvblobslibConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${OPENCVBLOBSLIB_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(opencvblobslibConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/opencvblobslibConfig.cmake" @ONLY)
# ... for both
configure_file(opencvblobslibConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/opencvblobslibConfigVersion.cmake" @ONLY)

# Install the opencvblobslibConfig.cmake and opencvblobslibConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/opencvblobslibConfig.cmake"
  "${PROJECT_BINARY_DIR}/opencvblobslibConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT opencvblobslibTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
