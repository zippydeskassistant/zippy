target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui.c)

add_subdirectory(tabs)
add_subdirectory_ifdef(CONFIG_ZIPPY_THEME_DEFAULT themes)
add_subdirectory(topbar)
