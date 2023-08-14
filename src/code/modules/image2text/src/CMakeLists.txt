set(srcs
    ocs.cpp
    OCRLanguageModel.cpp)

add_library(Image2Text
    STATIC
    ${srcs})

add_definitions(-DIMAGE2TEXT_ENABLED)
target_compile_definitions(Image2Text PUBLIC IMAGE2TEXT_ENABLED)

target_link_libraries (Image2Text
    PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
#    Qt5::Qml
    Qt${QT_MAJOR_VERSION}::Concurrent
    Qt${QT_MAJOR_VERSION}::Quick)

if (TESSERACT_FOUND AND LEPTONICA_FOUND)
    target_link_libraries(Image2Text
    PRIVATE
        Tesseract::Tesseract
        Leptonica::Leptonica)
endif()

