.RECIPEPREFIX := >

CXX = g++

CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Werror -Wshadow -MMD -MP

SRC_DIR = src
BUILD_DIR = build

TARGET = HackAssembler

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS)
>$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
>mkdir -p $(BUILD_DIR)
>$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
>rm -rf $(BUILD_DIR)/*
>rm -f $(TARGET)

-include $(DEPS)

.PHONY: clean