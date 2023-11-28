# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS +=

 

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

ifdef DEBUG
        CXXFLAGS += -g
endif


# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES  = res/fonts/Nunito-Bold.ttf res/Ubuntu\ Condensed\ 400.ttf res/Bravura.otf res/ModeScaleQuant-light.svg
DISTRIBUTABLES += res/fonts/DSEG7ClassicMini-Bold.ttf res/Meander-light.svg res/TinyPJ301M.svg res/ModeScaleProgressions-light.svg
DISTRIBUTABLES += $(wildcard LICENSE*)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk


