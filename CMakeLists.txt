cmake_minimum_required(VERSION 2.8.11)
project(perc_sample)

if(CMAKE_CL_64)
  set(PLATFORM "x64")
else(CMAKE_CL_64)
  set(PLATFORM "Win32")
endif(CMAKE_CL_64)

foreach(flag_var CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
  if(${flag_var} MATCHES "/MD")
    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
  endif()
endforeach()

#OpenCV
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv/build)
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

#PCSDK
set(PCSDK_DIR $ENV{PCSDK_DIR} CACHE PATH "Intel Perceptual Computing SDK Directory")
set(PCSDK_INCLUDE_DIRS
  ${PCSDK_DIR}/include
  ${PCSDK_DIR}/sample/common/include
)
set(PCSDK_LIBRARY_DIR ${PCSDK_DIR}/lib/${PLATFORM})
set(PCSDK_LIBRARIES
  debug "${PCSDK_LIBRARY_DIR}/libpxc_d.lib"
  optimized "${PCSDK_LIBRARY_DIR}/libpxc.lib"
)

add_subdirectory(wave)