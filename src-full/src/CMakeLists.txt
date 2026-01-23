set(fcitx_vmk_sources 
vmk.cpp
)

# 1. TẠO TARGET 'bamboo' (Dùng tên này để tránh lỗi CMake)
add_library(vmk MODULE ${fcitx_vmk_sources}) 

# 2. ĐẶT TÊN OUTPUT FILE: Giữ target là 'bamboo' nhưng ép tên file output thành 'vmk'
set_target_properties(vmk PROPERTIES OUTPUT_NAME "vmk")

# 3. Liên kết với target 'bamboo'
target_link_libraries(vmk Fcitx5::Core Fcitx5::Config Bamboo::Core Pthread::Pthread X11::X11) 

# 4. Include Directories cho target 'bamboo'
target_include_directories(vmk PRIVATE ${PROJECT_BINARY_DIR})

# 5. Cài đặt target 'bamboo'
install(TARGETS vmk DESTINATION "${CMAKE_INSTALL_LIBDIR}/fcitx5")

# ... (Các dòng cấu hình và cài đặt file addon giữ nguyên)
fcitx5_translate_desktop_file(vmk.conf.in vmk.conf) 
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vmk.conf" DESTINATION "${CMAKE_INSTALL_DATADIR}/fcitx5/inputmethod")
configure_file(vmk-addon.conf.in.in vmk-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/vmk-addon.conf.in" vmk-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vmk-addon.conf" RENAME vmk.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")
