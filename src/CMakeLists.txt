# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

if(NOT ANDROID)
    add_subdirectory(code/modules/image2text)
    add_subdirectory(code/modules/opencv)
endif()

add_library(MauiKitImageTools${MAUI_MAJOR_VERSION} SHARED)

ecm_add_qml_module(MauiKitImageTools${MAUI_MAJOR_VERSION}
    VERSION 1.0
    URI "org.mauikit.imagetools"
    CLASS_NAME ImageToolsPlugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
    "org.mauikit.controls")

## file headers to install
set(headers
    code/exiv2extractor.h
    code/geolocation/city.h
    code/geolocation/cities.h
    code/moduleinfo.h
    code/textscanner.h)

ecm_target_qml_sources(MauiKitImageTools${MAUI_MAJOR_VERSION} SOURCES
    controls.${QT_MAJOR_VERSION}/ImageViewer.qml
    controls.${QT_MAJOR_VERSION}/ImageInfoDialog.qml
    controls.${QT_MAJOR_VERSION}/ImageEditor.qml)

ecm_target_qml_sources(MauiKitImageTools${MAUI_MAJOR_VERSION} PATH private SOURCES
    controls.${QT_MAJOR_VERSION}/private/ColourBar.qml
    controls.${QT_MAJOR_VERSION}/private/Ruler.qml
    controls.${QT_MAJOR_VERSION}/private/TransformationBar.qml)

# qt_add_resources(imagetools_QML_QRC resources.qrc)

target_sources(MauiKitImageTools${MAUI_MAJOR_VERSION}
    PRIVATE

    code/imagetools_plugin.cpp
    code/imagetools_plugin.h

    code/picinfomodel.cpp
    code/picinfomodel.h

    code/imagemetadata.cpp
    code/imagemetadata.h

    code/exiv2extractor.cpp
    code/exiv2extractor.h

    code/geolocation/city.cpp
    code/geolocation/city.h

    code/geolocation/cities.cpp
    code/geolocation/cities.h

    code/geolocation/citiesdb.cpp
    code/geolocation/citiesdb.h

    code/geolocation/kdtree.cpp
    code/geolocation/kdtree.hpp

    code/moduleinfo.cpp

    code/textscanner.cpp
    code/textscanner.h
    data/cities.db)

target_sources(MauiKitImageTools${MAUI_MAJOR_VERSION}plugin
    PRIVATE

    code/imagetools_plugin.cpp
    code/imagetools_plugin.h

    code/picinfomodel.cpp
    code/picinfomodel.h

    code/imagemetadata.cpp
    code/imagemetadata.h

    data/assets.qrc)

add_library(MauiKit${MAUI_MAJOR_VERSION}::ImageTools ALIAS MauiKitImageTools${MAUI_MAJOR_VERSION})

generate_export_header(MauiKitImageTools${MAUI_MAJOR_VERSION} BASE_NAME ImageTools)
set_target_properties(MauiKitImageTools${MAUI_MAJOR_VERSION} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME "ImageTools")

target_include_directories(MauiKitImageTools${MAUI_MAJOR_VERSION}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/ImageTools>")

target_include_directories(MauiKitImageTools${MAUI_MAJOR_VERSION} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(ANDROID)

else()
    target_link_libraries(MauiKitImageTools${MAUI_MAJOR_VERSION}
        PRIVATE
        Image2Text)

    target_link_libraries(MauiKitImageTools${MAUI_MAJOR_VERSION}plugin
        PRIVATE
        Image2Text)

    ecm_target_qml_sources(MauiKitImageTools${MAUI_MAJOR_VERSION} PATH image2text SOURCES
        controls.${QT_MAJOR_VERSION}/image2text/OCRPage.qml)
endif()

target_link_libraries(MauiKitImageTools${MAUI_MAJOR_VERSION}
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Sql
    ${EXIV2_LIBRARIES}
    Qt${QT_MAJOR_VERSION}::Positioning
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::I18n
    MauiKit${MAUI_MAJOR_VERSION})

target_link_libraries(MauiKitImageTools${MAUI_MAJOR_VERSION}plugin
    PRIVATE
    MauiKit${MAUI_MAJOR_VERSION}
    MauiKitImageTools${MAUI_MAJOR_VERSION})

ecm_finalize_qml_module(MauiKitImageTools${MAUI_MAJOR_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR})

install(TARGETS MauiKitImageTools${MAUI_MAJOR_VERSION} EXPORT MauiKitImageTools${MAUI_MAJOR_VERSION}Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/imagetools_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/ImageTools
    COMPONENT Devel)

if(ANDROID)
    install (FILES data/cities.db DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/imagetools)
else()
    install (FILES data/cities.db DESTINATION ${KDE_INSTALL_DATADIR}/org/mauikit/imagetools)
endif()

