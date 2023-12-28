# project and bin directory
PROJ_DIR := ./../../
BIN_LOC := lib/

# compilation related
LIB_NAME := renderer
CXX := g++
CXXFLAGS := -std=c++17

# c compilation related
C_COMPILE := gcc
C_COMPILEFLAGS := -std=c17

# library path
LIB_PATH := $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/$(LIB_NAME).a

# library .hpp and .cpp and .c
LIB_HEADER := $(shell find . -name "*.hpp")
LIB_HEADER_DST := $(patsubst ./%,$(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/headers/%, $(LIB_HEADER))

LIB_CPP := $(shell find . -name "*.cpp")
LIB_OBJ := $(patsubst ./%.cpp, $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/obj/%.o, $(LIB_CPP))

LIB_C := $(shell find . -name "*.c")
LIB_C_OBJ := $(patsubst ./%.c, $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/obj/%.o, $(LIB_C))

# dependencies
DEPS_INC := -I$(PROJ_DIR)deps

.PHONY: all
all: $(LIB_PATH) $(LIB_HEADER_DST)

.PHONY: clean
clean:
	rm -rf $(PROJ_DIR)lib/renderer

.PHONY: copy_headers
copy_headers: $(LIB_HEADER_DST)

# create the folders
.PHONY: create_folder
create_folder:
	# create the bin folder
	mkdir -p $(PROJ_DIR)$(BIN_LOC)
	# create the folder to store everything to do with this lib
	mkdir -p $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/

# compile the cpp source code
$(LIB_OBJ) : $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/obj/%.o : ./%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $(DEPS_INC) -o $@ $<

# compile the c source code (mostly from extlibs)
$(LIB_C_OBJ) : $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/obj/%.o : ./%.c
	mkdir -p $(dir $@)
	$(C_COMPILE) $(C_COMPILEFLAGS) -c $(DEPS_INC) -o $@ $<

# compile the library
$(LIB_PATH) : $(LIB_OBJ) $(LIB_C_OBJ)
	ar rcs $@ $^

# copy the header files along with the hierarchy
$(LIB_HEADER_DST) : $(PROJ_DIR)$(BIN_LOC)$(LIB_NAME)/headers/%.hpp : ./%.hpp
	mkdir -p $(dir $@)
	cp $< $@
