#------------------------------------------------------------------------------
# Include source and build directories
#------------------------------------------------------------------------------
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}
)

#------------------------------------------------------------------------------
# External dependencies
#------------------------------------------------------------------------------
#HDF5
find_package(HDF5 NO_MODULE NAMES hdf5 COMPONENTS C shared)
if(HDF5_FOUND)
message(STATUS "Found hdf5 library")
  set(HDF5_C_SHARED_LIBRARY hdf5-shared)
  set(HDF5_IOTEST_EXT_INCLUDE_DEPENDENCIES
      ${HDF5_IOTEST_EXT_INCLUDE_DEPENDENCIES}
      ${HDF5_INCLUDE_DIR}
  )
  set(HDF5_IOTEST_EXT_PKG_DEPENDENCIES
      ${HDF5_IOTEST_EXT_PKG_DEPENDENCIES}
      ${HDF5_C_SHARED_LIBRARY}
  )
else()
  message(STATUS "NOT found hdf5 library")
endif()

# MPI
# Temporary workaround because HDF5 does not pull MPI dependency
find_package(MPI REQUIRED)
if(MPI_FOUND)
  set(ENABLE_MPI 1)
  set(HDF5_IOTEST_EXT_INCLUDE_DEPENDENCIES
      ${HDF5_IOTEST_EXT_INCLUDE_DEPENDENCIES}
      ${MPI_INCLUDE_PATH}
  )
  set(HDF5_IOTEST_EXT_PKG_DEPENDENCIES
      ${HDF5_IOTEST_EXT_PKG_DEPENDENCIES}
      ${MPI_LIBRARIES}
  )
else()
  message(FATAL_ERROR "Could not find MPI.")
endif()

#UUID
find_package(PkgConfig)
pkg_search_module(UUID REQUIRED uuid)
set(HDF5_IOTEST_BUILD_INCLUDE_DEPENDENCIES
    ${HDF5_IOTEST_BUILD_INCLUDE_DEPENDENCIES}
    ${UUID_INCLUDE_DIRS}
)
set(HDF5_IOTEST_EXT_PKG_DEPENDENCIES
    ${HDF5_IOTEST_EXT_PKG_DEPENDENCIES}
    ${UUID_LIBRARIES}
)

#------------------------------------------------------------------------------
# Set sources
#------------------------------------------------------------------------------
set(HDF5_IOTEST_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/configuration.c
  ${CMAKE_CURRENT_SOURCE_DIR}/dataset.c
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_iotest.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ini.c
  ${CMAKE_CURRENT_SOURCE_DIR}/read_test.c
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.c
  ${CMAKE_CURRENT_SOURCE_DIR}/write_test.c
)

#------------------------------------------------------------------------------
# Libraries
#------------------------------------------------------------------------------
add_executable(hdf5_iotest ${HDF5_IOTEST_SRCS})

target_include_directories(hdf5_iotest
  PUBLIC  "$<BUILD_INTERFACE:${HDF5_IOTEST_BUILD_INCLUDE_DEPENDENCIES}>"
)
target_include_directories(hdf5_iotest
  SYSTEM PUBLIC ${HDF5_IOTEST_EXT_INCLUDE_DEPENDENCIES}
)
target_link_libraries(hdf5_iotest
  ${HDF5_IOTEST_EXT_PKG_DEPENDENCIES}
)
