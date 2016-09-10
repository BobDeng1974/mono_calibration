cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(OpenCV REQUIRED)

add_executable(split split.cpp)
target_link_libraries(split 
${OpenCV_LIBS})

add_executable(mono_calibration_auto mono_calibration_auto.cpp camera_calibrate.cpp)
target_link_libraries(mono_calibration_auto 
${OpenCV_LIBS})
