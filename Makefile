# Project Name
TARGET = vocodr

# Sources
CPP_SOURCES = vocodr.cpp

# Library Locations (place this project next to DaisyExamples)
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
