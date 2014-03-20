file(GLOB INC *.h)
file(GLOB SRC *.cpp)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${PCSDK_INCLUDE_DIRS}
)

add_executable(wave
  ${INC}
  ${SRC}
  ${PCSDK_DIR}/sample/common/src/util_capture.cpp
)
target_link_libraries(wave
  ${OpenCV_LIBS}
  ${PCSDK_LIBRARIES}
)