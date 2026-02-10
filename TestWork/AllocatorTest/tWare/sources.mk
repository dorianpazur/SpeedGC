
# automatically includes the contents of this directory

CW_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))

SOURCES := $(SOURCES) ./$(CW_SUBDIR)
# we're not including headers for included libraries as they're accessed using subdirs

#include subdirectories
include $(wildcard ./$(CW_SUBDIR)*/sources.mk)
