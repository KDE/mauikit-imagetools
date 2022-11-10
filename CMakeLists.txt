# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(KF5_VERSION "5.70.0")
set(REQUIRED_QT_VERSION 5.15.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(ImageTools LANGUAGES CXX VERSION 2.2.1)

find_package(ECM ${KF5_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

option(QUICK_COMPILER "Use QtQuick compiler to improve performance" TRUE)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(ECMPoQmTools)
include(ECMQMLModules)
include(ECMSetupVersion)
include(CMakePackageConfigHelpers)
include(KDEClangFormat)
include(KDEPackageAppTemplates)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/MauiKitImageTools")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/MauiKitImageToolsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitImageToolsConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS KF5_INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitImageToolsConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitImageToolsConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT MauiKitImageToolsTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE MauiKitImageToolsTargets.cmake
    NAMESPACE MauiKit::)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX ImageTools
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/imagetools_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/MauiKitImageToolsConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/imagetools_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit/FileBrowsing
    COMPONENT Devel)

find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS Core Quick Qml Sql Positioning REQUIRED)
find_package(LibExiv2 0.21 REQUIRED)
#find_package(ImageMagick)

if(ANDROID)
    find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS AndroidExtras)
endif()

find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n CoreAddons)
find_package(MauiKit REQUIRED)

if(QUICK_COMPILER)
    find_package(Qt5QuickCompiler)
    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Compile QML at build time"
        TYPE OPTIONAL
        )
endif()

ecm_find_qmlmodule(org.kde.kquickimageeditor 1.0)
find_package(KQuickImageEditor COMPONENTS)
set_package_properties(KQuickImageEditor PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Simple image editor for QtQuick applications"
    URL "https://invent.kde.org/libraries/kquickimageeditor/"
    PURPOSE "Add image editing capability to image attachments"
    )

kde_enable_exceptions()

add_subdirectory(src)

    add_definitions(-DTRANSLATION_DOMAIN="mauikitimagetools")
    ki18n_install(po)
    
    file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
