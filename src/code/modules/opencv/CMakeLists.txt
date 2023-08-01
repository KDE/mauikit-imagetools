project(OpenCVLib VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library (OpenCVLib STATIC ${srcs})
include_directories( ${OpenCV_INCLUDE_DIRS} )
#add_executable( DisplayImage DisplayImage.cpp )
target_link_libraries( OpenCVLib ${OpenCV_LIBS} )
