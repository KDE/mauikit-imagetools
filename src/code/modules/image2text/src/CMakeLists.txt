# include_directories(${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(Image2Text STATIC)

target_sources(Image2Text
    PRIVATE
    ocs.cpp
    OCRLanguageModel.cpp)

add_definitions(-DIMAGE2TEXT_ENABLED)
target_compile_definitions(Image2Text PUBLIC IMAGE2TEXT_ENABLED)

target_link_libraries (Image2Text
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Gui

    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Concurrent
    Qt${QT_MAJOR_VERSION}::Quick)

if (TESSERACT_FOUND AND LEPTONICA_FOUND)
    target_link_libraries(Image2Text
        PRIVATE
        OpenCVLib
        Tesseract::Tesseract
        Leptonica::Leptonica
    )
endif()

generate_export_header(Image2Text BASE_NAME Image2Text)

#     set_target_properties(Image2Text PROPERTIES
#     VERSION ${PROJECT_VERSION}
#     SOVERSION ${PROJECT_VERSION_MAJOR}
#     EXPORT_NAME "Image2Text")
# 
# 
# target_include_directories(Image2Text PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")
