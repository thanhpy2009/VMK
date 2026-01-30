cmake_minimum_required(VERSION 3.6)

project(fcitx5-vmk VERSION 0.9) 

set(REQUIRED_FCITX_VERSION 5.1.7)
find_package(ECM 1.0.0 REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(FeatureSummary)
include(GNUInstallDirs)
include(ECMUninstallTarget)

find_package(Fcitx5Core ${REQUIRED_FCITX_VERSION} REQUIRED)
find_package(Fcitx5Module REQUIRED COMPONENTS TestFrontend)
find_package(Pthread REQUIRED)
find_package(X11 REQUIRED)
include("${FCITX_INSTALL_CMAKECONFIG_DIR}/Fcitx5Utils/Fcitx5CompilerSettings.cmake")

add_definitions(-DFCITX_GETTEXT_DOMAIN=\"fcitx5-vmk\") 


add_definitions(-DFCITX_INSTALL_LOCALEDIR=\"\")

add_subdirectory(bamboo)
add_subdirectory(src)
add_subdirectory(data)

configure_file(org.fcitx.Fcitx5.Addon.VMK.metainfo.xml.in
    org.fcitx.Fcitx5.Addon.VMK.metainfo.xml COPYONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.fcitx.Fcitx5.Addon.VMK.metainfo.xml" 
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
