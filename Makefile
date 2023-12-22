LDFLAGS := -static -lglfw3 -lassimp -lzlib 
ifeq ($(OS),Windows_NT)
  TARGET_EXEC := demo.exe
  PATH_SEPARATOR = \\
  SRC_DIRS := .\\
  SRCS := $(shell powershell gci -r -Path ${SRC_DIRS} -Include "*.cpp", "*.c" -Name)
  INC_DIRS := $(shell powershell gci -r -Path ${SRC_DIRS} -Filter "*include" -Name)
  LDFLAGS += -lgdi32
define CREATE_DIR
	if not exist $(1) mkdir $(1)
endef
  CLEAN_DIR := rmdir /s /q $(BUILD_DIR)
else
  TARGET_EXEC := demo
  PATH_SEPARATOR := /
  SRC_DIRS := ./src ./engine ./include 
  SRCS := $(shell find ${SRC_DIRS} -name '*.cpp' -or -name '*.c')
  INC_DIRS := $(shell find $(SRC_DIRS) -type d)
define CREATE_DIR 
	mkdir -p $(1)
endef
  CLEAN_DIR := rm -r $(BUILD_DIR)
endif


BUILD_DIR := .$(PATH_SEPARATOR)build

OBJS := $(SRCS:%=$(BUILD_DIR)$(PATH_SEPARATOR)%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CXX := g++
CC := gcc
CPPFLAGS := $(INC_FLAGS) -MMD -MP 
CXXFLAGS := -std=c++17 -g3 -O0 -xc++

$(BUILD_DIR)$(PATH_SEPARATOR)$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) 

$(BUILD_DIR)$(PATH_SEPARATOR)%.cpp.o: %.cpp
	$(call CREATE_DIR,$(dir $@))
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)$(PATH_SEPARATOR)%.c.o: %.c
	$(call CREATE_DIR,$(dir $@))
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(BUILD_DIR)$(PATH_SEPARATOR)$(TARGET_EXEC)
	$(BUILD_DIR)$(PATH_SEPARATOR)$(TARGET_EXEC)

.PHONY: clean
clean: 
	rmdir /s /q $(BUILD_DIR)

.PHONY: debug
debug:
	echo $(INC_FLAGS)
	echo $(SRCS)
	echo $(OBJS)
	echo $(OS)
	
-include $(DEPS)
