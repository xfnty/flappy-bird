MAKEFLAGS=--no-print-directory --quiet

ifdef OS
	path = $(subst /,\,$1)
else
	path = $1
endif

PROJECT_NAME=flappy-bird
CMAKE_DIR:=.cmake
OUTPUT_DIR:=bin
EXE=$(call path, $(OUTPUT_DIR)/$(PROJECT_NAME))

CONFIGURE:=configure c
BUILD:=build b
RUN:=run r
DEBUG:=debug d

all: configure build run

$(CONFIGURE):
	echo ----- Configuring -----
	mkdir -p $(CMAKE_DIR)
	cmake -B $(CMAKE_DIR) -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
	cp -f $(call path, $(CMAKE_DIR)/compile_commands.json) compile_commands.json

$(BUILD):
	echo ----- Building -----
	mkdir -p $(OUTPUT_DIR)
	cmake --build $(CMAKE_DIR)

$(RUN):
	echo ----- Running -----
	cp -rf assets $(OUTPUT_DIR)
	cd $(OUTPUT_DIR)
	$(EXE)

$(DEBUG):
	echo ----- Debugging -----
	cp -rf assets $(OUTPUT_DIR)
	cd $(OUTPUT_DIR)
	gdb -q --return-child-result $(EXE)

clean:
	git clean -Xdfq

.PHONY=$(CONFIGURE) $(BUILD) $(RUN) $(DEBUG) clean
