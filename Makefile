SHELL := pwsh
.SHELLFLAGS := -Command

TARGET_EXEC := OpenGl.exe

BUILD_DIR := .\build
SRC_DIRS := .\



SRCS := $(shell powershell gci -r -Path ${SRC_DIRS} -Include "*.cpp", "*.c" -Name)

OBJS := $(SRCS:%=$(BUILD_DIR)\\%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell powershell gci -r -Path ${SRC_DIRS} -Filter "*include" -Name)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP 
CXXFLAGS := -std=c++17 -g3 -O2 -xc++
LDFLAGS := -static -L.\engine\dependencies\lib -lglfw3 -lgdi32 -lopengl32 

$(BUILD_DIR)\$(TARGET_EXEC): $(OBJS)
	g++ $(OBJS) -o $@ $(LDFLAGS) 

$(BUILD_DIR)\\%.cpp.o: %.cpp
	if not exist $(dir $@) mkdir $(dir $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)\\%.c.o: %.c
	if not exist $(dir $@) mkdir $(dir $@)
	gcc $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(BUILD_DIR)\$(TARGET_EXEC)
	$(BUILD_DIR)\$(TARGET_EXEC)

.PHONY: clean
clean: 
	rmdir /s /q $(BUILD_DIR)

.PHONY: debug
debug:
	echo $(INC_FLAGS)
	echo $(SRCS)
	echo $(OBJS)
	
-include $(DEPS)
