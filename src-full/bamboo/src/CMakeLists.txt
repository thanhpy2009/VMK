
set( fcitx_bamboo_sources
    bamboo.cpp
)

add_fcitx5_addon(bamboo ${fcitx_bamboo_sources})
target_link_libraries(bamboo Fcitx5::Core Fcitx5::Config Bamboo::Core Pthread::Pthread)
target_include_directories(bamboo PRIVATE ${PROJECT_BINARY_DIR})
install(TARGETS bamboo DESTINATION "${CMAKE_INSTALL_LIBDIR}/fcitx5")
fcitx5_translate_desktop_file(bamboo.conf.in bamboo.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/bamboo.conf" DESTINATION "${CMAKE_INSTALL_DATADIR}/fcitx5/inputmethod")
configure_file(bamboo-addon.conf.in.in bamboo-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/bamboo-addon.conf.in" bamboo-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/bamboo-addon.conf" RENAME bamboo.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

