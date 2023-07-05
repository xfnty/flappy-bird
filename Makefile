MAKEFLAGS=--no-print-directory --quiet

ifdef OS
	path = $(subst /,\,$1)
	OS_NAME:=windows
	OS_ARCH:=x86_x64
else
	path = $1
	OS_NAME:=$(shell uname -s | tr A-Z a-z)
	OS_ARCH:=$(shell uname -p)
endif

PROJECT_NAME=flappy-bird
CMAKE_DIR:=.cmake
OUTPUT_DIR:=bin
EXE=$(call path, $(OUTPUT_DIR)/$(PROJECT_NAME))

CONFIGURE:=configure c
BUILD:=build b
RUN:=run r
DEBUG:=debug d
DIST:=dist

_CONFIGURE:=cmake -B $(CMAKE_DIR) -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
_COPY_CCOMMANDS:=cp -f $(call path, $(CMAKE_DIR)/compile_commands.json) compile_commands.json
_BUILD:=cmake --build $(CMAKE_DIR) -j
_COPY_ASSETS:=cp -rf assets $(OUTPUT_DIR)

all: $(CMAKE_DIR) build run

$(CONFIGURE):
	echo ----- Configuring -----
	mkdir -p $(CMAKE_DIR)
	$(_CONFIGURE)
	-$(_COPY_CCOMMANDS)

$(BUILD):
	echo ----- Building -----
	mkdir -p $(OUTPUT_DIR)
	$(_BUILD)
	$(_COPY_ASSETS)

$(RUN):
	echo ----- Running -----
	$(_COPY_ASSETS)
	cd $(OUTPUT_DIR)
	$(EXE)

$(DEBUG):
	echo ----- Debugging -----
	$(_COPY_ASSETS)
	cd $(OUTPUT_DIR)
	gdb -q --return-child-result $(EXE)

$(DIST): $(OUTPUT_DIR)
	echo ----- Packing distribution -----
	$(_COPY_ASSETS)
	-rm -f $(PROJECT_NAME)-$(OS_NAME)-$(OS_ARCH).zip
	zip -9 -r $(PROJECT_NAME)-$(OS_NAME)-$(OS_ARCH).zip $(call path, $(OUTPUT_DIR)/*)

$(CMAKE_DIR):
	echo ----- Configuring -----
	mkdir -p $(CMAKE_DIR)
	$(_CONFIGURE)
	-$(_COPY_CCOMMANDS)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)
	$(_BUILD)

clean:
	git clean -Xdfq

.PHONY=$(CONFIGURE) $(BUILD) $(RUN) $(DEBUG) $(DIST) clean
