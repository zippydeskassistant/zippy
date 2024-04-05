zephyr_include_directories(.)
zephyr_include_directories(themes)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui.c)
target_sources_ifdef(CONFIG_ZIPPY_THEME_DEFAULT app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/themes/zippy_theme_default.c)

target_sources_ifdef(CONFIG_ZIPPY_GRUB_COMMANDER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tabs/tab_grub_commander.c)
