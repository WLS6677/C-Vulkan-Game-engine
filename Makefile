
SRCPATHS = src/*

INCLUDEPATHS = -Iinclude -Isrc -I$(VULKAN_SDK)/Include
LIBPATHS = -Llib -L$(VULKAN_SDK)/Lib
LIBS = -lglfw3 -lgdi32 -lopengl32 -lvulkan-1

SRC = $(wildcard $(SRCPATHS))
OBJECTS = $(SRC:src/%.cpp=build/%.o)

EXECUTABLE_NAME = GameEngine

ifeq ($(OS),Windows_NT)
    EXECUTABLE_NAME = GameEngine.exe
endif

CC = g++
LD = g++

DEBUGFLAGS = -g -DWL_DEBUG
CFLAGS = -std=c++17 -D_XOPEN_SOURCE=700 -fwrapv \
         -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing


OPT = -O3
FILE_NUM = 0

#compiling source files
build/%.o: src/%.cpp
	@if not exist build mkdir build
	@$(CC) -c -o $@ $< $(INCLUDEPATHS) $(CFLAGS) $(OPT)

#linking the compiled files
$(EXECUTABLE_NAME): $(OBJECTS)
	@echo Linking with $(LD)...
	@$(LD) $(OBJECTS) -o $(EXECUTABLE_NAME) $(CFLAGS) $(LIBPATHS) $(LIBS) -lm
	@echo Successfully built: $(EXECUTABLE_NAME)

build: $(EXECUTABLE_NAME)

debug: CFLAGS += $(DEBUGFLAGS) 
debug: OPT = -O0
debug: build

clean:
	@if exist build\* del /s /q build\*
	@if exist $(EXECUTABLE_NAME) del $(EXECUTABLE_NAME)
	@if not exist build mkdir build
	@cls
cleanbuild: clean build