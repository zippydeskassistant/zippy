target_sources_ifdef(CONFIG_ZIPPY_AUDIO_CONTROLLER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tab_audio_controller.c)
target_sources_ifdef(CONFIG_ZIPPY_AUDIO_PLAYER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tab_audio_player.c)
target_sources_ifdef(CONFIG_ZIPPY_GRUB_COMMANDER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tab_grub_commander.c)
target_sources_ifdef(CONFIG_ZIPPY_MACROS app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tab_macros.c)