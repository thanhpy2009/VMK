install(DIRECTORY scalable DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor"
        PATTERN .* EXCLUDE
        PATTERN *~ EXCLUDE)

install(FILES vietnamese.cm.dict DESTINATION "${FCITX_INSTALL_PKGDATADIR}/bamboo")