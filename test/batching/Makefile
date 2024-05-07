PROJ_DIR := ./../../
BIN_LOC := testbin/
APP_NAME := $(PROJ_DIR)$(BIN_LOC)test_batching
CXX := g++
CXXFLAGS := -std=c++17

OS := $(shell uname)
ifeq ($(OS), Darwin)
	NATIVE_LIBS := -lm -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
else ifeq ($(OS), Linux)
	NATIVE_LIBS := -lm -lGL -lm -lX11 -lXrandr -lXinerama -ldl
endif

EXT_LIBS := $(shell find $(PROJ_DIR)deps/ -maxdepth 2 -name "*.a")
LIBS := $(PROJ_DIR)/lib/renderer/renderer.a
INC :=\
	-I$(PROJ_DIR)lib/renderer/headers\
	-I$(PROJ_DIR)deps\

.PHONY: all
all: $(APP_NAME)

.PHONY: run
run: $(APP_NAME)
	$(APP_NAME)

.PHONY: create_folder
create_folder:
	mkdir -p $(PROJ_DIR)$(BIN_LOC)

.PHONY: clean
clean:
	rm -rf $(APP_NAME)

$(APP_NAME) : main.cpp | create_folder
	$(CXX) $(CXXFLAGS) $(INC) -o $@ $^ $(LIBS) $(EXT_LIBS) $(NATIVE_LIBS)
