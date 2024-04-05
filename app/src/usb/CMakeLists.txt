zephyr_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/usb.c)

# TODO: we should only icnlude keyboard and raw if they have a definition in the devicetree, individually.
target_sources_ifdef(CONFIG_DT_HAS_ZEPHYR_HID_DEVICE_ENABLED app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/keyboard.c)
target_sources_ifdef(CONFIG_DT_HAS_ZEPHYR_HID_DEVICE_ENABLED app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/raw_hid.c)
target_sources_ifdef(CONFIG_ZIPPY_GRUB_COMMANDER app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/msc.c )