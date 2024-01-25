MODULE := engines/comet

MODULE_OBJS = \
	archivesManager.o \
	audioManager.o \
	comet.o \
	CometXorSerializer.o \
	console.o \
	gameManager.o \
	gameState.o \
	graphicsManager.o \
	metaengine.o \
	mouseManager.o \
	scriptsManager.o \
	speechManager.o \
	textManager.o \

# This module can be built as a plugin
ifeq ($(ENABLE_COMET), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
