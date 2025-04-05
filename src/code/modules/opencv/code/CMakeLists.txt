set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(srcs
    preprocessimage.cpp
    mycroplayer.cpp
    cvmatandqimage.cpp
    convertimage.hpp)

add_library (OpenCVLib STATIC)

target_sources(OpenCVLib PRIVATE ${srcs})

include_directories( ${OpenCV_INCLUDE_DIRS})

# target_include_directories(OpenCVLib INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/OpenCVLib>")

target_include_directories(OpenCVLib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

#add_executable( DisplayImage DisplayImage.cpp )
target_link_libraries( OpenCVLib
    PUBLIC
    ${OpenCV_LIBS}

    PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui)

generate_export_header(OpenCVLib BASE_NAME OpenCVLib)
