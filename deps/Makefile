DEP_STATIC_LIB := \
GLFW/libglfw3.a\

# modify above when adding new libs

OS := $(shell uname)
ifeq ($(OS), Darwin)
	SEARCH_FOLDER := mac
endif

.PHONY: all
all: $(DEP_STATIC_LIB)

.PHONY: clean
clean:
	rm -rf $(DEP_STATIC_LIB)

$(DEP_STATIC_LIB):
	cp $(dir $@)/$(SEARCH_FOLDER)/$(notdir $@) $(dir $@)
