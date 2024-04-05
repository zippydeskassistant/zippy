zephyr_include_directories(.)
target_sources_ifdef(CONFIG_ZIPPY_GRUB_COMMANDER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/grub_commander.c)