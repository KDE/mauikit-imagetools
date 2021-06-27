# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

ecm_create_qm_loader(imagetools_QM_LOADER libimagetoolsplugin_qt)

set(
    sources
    code/imagetools_plugin.cpp
    code/picinfomodel.cpp
    code/exiv2extractor.cpp
    code/geolocation/city.cpp
    code/geolocation/cities.cpp
    code/geolocation/kdtree.cpp
    resources.qrc
    ${imagetools_QM_LOADER}
    )

set(
    headers
    code/picinfomodel.h
    code/imagetools_plugin.h
    code/exiv2extractor.h
    code/geolocation/city.h
    code/geolocation/cities.h
    code/geolocation/kdtree.hpp
    )

set(
    pluginData
    controls/qmldir
    )

add_library(
    MauiKitImageTools
    SHARED
    ${sources} ${pluginData}
    )

add_library(MauiKit::ImageTools ALIAS MauiKitImageTools)

generate_export_header(MauiKitImageTools BASE_NAME ImageTools)
set_target_properties(MauiKitImageTools PROPERTIES
    VERSION ${ImageTools_VERSION_STRING}
    SOVERSION ${ImageTools_SOVERSION}
    EXPORT_NAME ImageTools
    )

if(UNIX AND NOT ANDROID)
    target_link_libraries(
        MauiKitImageTools
        PRIVATE
        KF5::ConfigCore
        )
endif()

target_include_directories(MauiKitImageTools
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MauiKit/ImageTools>")

target_include_directories(MauiKitImageTools PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(
    MauiKitImageTools

    PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
    Qt5::Sql
    LibExiv2::LibExiv2
    Qt5::Positioning
    KF5::CoreAddons
    MauiKit
    )

install(TARGETS MauiKitImageTools EXPORT MauiKitImageToolsTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers}
    ${CMAKE_CURRENT_BINARY_DIR}/imagetools_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit/ImageTools
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_BINARY_DIR}/bin/org/mauikit/imagetools/
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/mauikit/imagetools/
        COMMAND ${CMAKE_COMMAND} -E
        copy $<TARGET_FILE:MauiKitImageTools> ${CMAKE_BINARY_DIR}/bin/org/mauikit/imagetools/
        )

    install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/imagetools)

endif()

install(TARGETS MauiKitImageTools DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/imagetools )
