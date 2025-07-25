BUILD_DIR = build
TARGET = $(BUILD_DIR)/riscv
LIB_TARGET = $(BUILD_DIR)/libriscv.a

CXX = g++
AR = ar
RANLIB = ranlib

CXXFLAGS = -std=c++17 -Wall -Werror -g -O2

MAIN_SOURCE = src/main.cpp
LIB_SOURCES = lib/lexer/src/lexer.cpp
# $(wildcard lib/*/src/*.cpp)

MAIN_OBJECT = $(BUILD_DIR)/main.o
LIB_OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(LIB_SOURCES)))

INCLUDE_DIRS = $(wildcard lib/*/include)
INC_FLAGS = $(foreach dir, $(INCLUDE_DIRS), -I./$(dir))
CXXFLAGS += $(INC_FLAGS)

GREEN = \033[032m
BLUE  = \033[036m
RESET = \033[0m

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): $(MAIN_OBJECT) $(LIB_TARGET)
	@echo "$(BLUE)Linking $(TARGET)...$(RESET)"
	$(CXX) $(CXXFLAGS) $(MAIN_OBJECT) $(LIB_TARGET) -o $@
	@echo "$(GREEN)Build complete$(RESET)"

$(LIB_TARGET): $(LIB_OBJECTS)
	@echo "$(BLUE)Creating static library $(LIB_TARGET)...$(RESET)"
	$(AR) rcs $@ $(LIB_OBJECTS)
	@echo "$(BLUE)Adding symbol index to $(LIB_TARGET)...$(RESET)"
	$(RANLIB) $@
	@echo "$(GREEN)Library created$(RESET)"

$(MAIN_OBJECT): $(MAIN_SOURCE) | $(BUILD_DIR)
	@echo "$(BLUE)Compiling $< to $@$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: lib/lexer/src/%.cpp | $(BUILD_DIR)
	@echo "$(BLUE)Compiling $< to $@$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: lib/parser/src/%.cpp | $(BUILD_DIR)
	@echo "$(BLUE)Compiling $< to $@$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: lib/mapper/src/%.cpp | $(BUILD_DIR)
	@echo "$(BLUE)Compiling $< to $@$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(BLUE)Cleaning build directory...$(RESET)"
	@rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d
	@rm -f $(TARGET) $(LIB_TARGET)
	@rmdir $(BUILD_DIR) 2>/dev/null || true
	@echo "$(GREEN)Cleanup complete$(RESET)"

-include $(MAIN_OBJECT:.o=.d) $(LIB_OBJECTS:.o=.d)
