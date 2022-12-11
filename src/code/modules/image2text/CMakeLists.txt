cmake_minimum_required(VERSION 3.16)

project(Image2Text VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(REQUIRED_QT_VERSION "5.15.2")
set(REQUIRED_KF5_VERSION "5.94")

find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

find_package(Tesseract 4)
find_package(Leptonica)

if (LEPTONICA_FOUND AND TESSERACT_FOUND)
   set(OCR_AVAILABLE 1)
   set(CMAKE_CXX_FLAGS "-fopenmp")
else()
   message(STATUS "Skanpage will be built without OCR as libraries are missing.")
endif()


find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Quick Concurrent)

find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS CoreAddons I18n)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
