SRC_DIR := part-3
OUT_DIR := build

OBJ_DIR := $(OUT_DIR)/.o
DEP_DIR := $(OUT_DIR)/.d

EXECUTABLE := $(OUT_DIR)/a.out

__generate_depenencies :=  -MMD -MP -I./$(SRC_DIR)

CXX := gcc

CXXFLAGS := -Wall -Wextra $(__generate_depenencies) -O2

LINKER_FLAGS := 


#################################################################
###                     T H E   B U I L D                     ###
#################################################################


SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(patsubst $(SRC_DIR)/%.c,$(DEP_DIR)/%.d,$(SRC_FILES))

-include $(DEP_FILES)

$(EXECUTABLE): $(OBJ_FILES)
	@mkdir -p $(OUT_DIR)
	@echo "linking $@"
	@$(CXX) $(LINKER_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	@echo "compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@mv $(OBJ_DIR)/$*.d $(DEP_DIR)/$*.d


#################################################################
###                 P H O N Y   T A R G E T S                 ###
#################################################################


PHONY_TARGETS := executable run clean help

.PHONY: $(PHONY_TARGETS)


executable: $(EXECUTABLE)

run: executable
	@echo "running $(EXECUTABLE)"
	@$(EXECUTABLE)

clean:
	rm -rf $(OUT_DIR)

help:
	@echo "Available targets:"
	@echo "  make ------- build the code"
	@echo "  make run --- build & run the code"
	@echo "  make clean - delete the build folder"
	@echo "  make help -- show this menu"
