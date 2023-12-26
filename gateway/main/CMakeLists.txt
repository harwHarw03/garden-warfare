# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main_esp_now_only.c")
# set(COMPONENT_SRCS "main_with_mqtt.c" "mqtt_pub.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
