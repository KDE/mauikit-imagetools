# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(KF5_VERSION "5.70.0")
set(REQUIRED_QT_VERSION 5.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(ImageTools LANGUAGES CXX VERSION 2.0.0)

find_package(ECM ${KF5_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

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
#include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
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
    SOVERSION 1
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/imagetools_version.h"
        DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}"
        COMPONENT Devel)
                
find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS Core Quick Sql Positioning REQUIRED)
find_package(LibExiv2 0.21 REQUIRED)

if(UNIX AND NOT ANDROID)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS Config)
endif()

find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n CoreAddons)
find_package(MauiKit REQUIRED)

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

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
