cmake_minimum_required (VERSION 3.10)
project (HDF5_IOTEST)

#------------------------------------------------------------------------------
# Verify data
#------------------------------------------------------------------------------
option(IOTEST_VERIFY_DATA OFF)
if(IOTEST_VERIFY_DATA)
  set(VERIFY_DATA 1)
endif()
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${PROJECT_BINARY_DIR}/config.h)

#------------------------------------------------------------------------------
# Disallow in-source build
#------------------------------------------------------------------------------
if("${HDF5_IOTEST_SOURCE_DIR}" STREQUAL "${HDF5_IOTEST_BINARY_DIR}")
  message(FATAL_ERROR
    "HDF5 IOTEST requires an out of source Build. "
    "Please create a separate binary directory and run CMake there.")
endif()

#------------------------------------------------------------------------------
# Set a default build type if none was specified
#------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (NOT "${CMAKE_C_FLAGS_DEBUG}" MATCHES ".*-fsanitize=address.*")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address"
       CACHE STRING "" FORCE)
  endif()
endif()

#-----------------------------------------------------------------------------
# Source
#-----------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
