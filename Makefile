#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := mangov2

EXTRA_COMPONENT_DIRS := third_party/esp/components
EXCLUDE_COMPONENTS := max7219 mcp23x17 led_strip max31865 ls7366r max31855 ads130e08

include $(IDF_PATH)/make/project.mk

