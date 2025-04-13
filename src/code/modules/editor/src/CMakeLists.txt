
# add_library(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION})
ecm_add_qml_module(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION}
    VERSION 1.0
    URI "org.mauikit.imagetools.editor"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
    "org.mauikit.controls")

target_sources(MauiKitImageToolsEditor4
  PRIVATE
    commands/transformcommand.h commands/transformcommand.cpp
)

ecm_target_qml_sources(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION} SOURCES
    controls/ImageEditor.qml)


ecm_target_qml_sources(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION} PATH private SOURCES
    controls/private/ColourBar.qml
    controls/private/Ruler.qml
    controls/private/TransformationBar.qml

    controls/BasicResizeHandle.qml
    controls/SelectionTool.qml
    controls/SelectionHandle.qml
    controls/SelectionBackground.qml
    controls/CropBackground.qml
    controls/RectangleCutout.qml)

target_sources(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION}
    PRIVATE
    imageitem.cpp
    imageitem.h

    imagedocument.cpp
    imagedocument.h

    commands/command.cpp
    commands/transformcommand.cpp
    commands/cropcommand.cpp
    commands/mirrorcommand.cpp
    commands/resizecommand.cpp
    commands/rotatecommand.cpp)

# target_sources(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION})plugin PRIVATE
# imageitem.cpp
# imageitem.h
#  imagedocument.cpp
# imagedocument.h)

set_target_properties(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME "ImageToolsEditor")

target_link_libraries(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION}
    PRIVATE
    OpenCVLib
    Qt6::Quick
    Qt6::Core)

# install(FILES
#     ${headers}
#     ${CMAKE_CURRENT_BINARY_DIR}/imagetools_export.h
#     DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/ImageTools
#     COMPONENT Devel)

ecm_finalize_qml_module(MauiKitImageToolsEditor${MAUI_MAJOR_VERSION} EXPORT MauiKitImageTools${MAUI_MAJOR_VERSION}Targets)
install(TARGETS MauiKitImageToolsEditor${MAUI_MAJOR_VERSION} EXPORT MauiKitImageTools${MAUI_MAJOR_VERSION}Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
