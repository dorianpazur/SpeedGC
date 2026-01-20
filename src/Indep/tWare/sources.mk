
# automatically includes the contents of this directory

CW_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))

SOURCES := $(SOURCES) ./$(CW_SUBDIR)
INCLUDES := $(INCLUDES) ./$(CW_SUBDIR)

#include subdirectories
include $(wildcard ./$(CW_SUBDIR)*/sources.mk)
