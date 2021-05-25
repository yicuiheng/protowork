TARGET_NAME = protowork

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -pedantic -DGLEW_STATIC
DEBUG_CXXFLAGS = -O0 -g3 -D_DEBUG -UNDEBUG
RELEASE_CXXFLAGS = -O3 -s -flto -DNDEBUG -U_DEBUG
TEST_CXX_FLAGS = $(DEBUG_CXXFLAGS) -DPROTOWORK_TEST
EXAMPLE_CXX_FLAGS = $(RELEASE_CXXFLAGS) -DPROTOWORK_EXAMPLE

AR = ar
AR_FLAGS = rcs

LDFLAGS = `pkg-config --libs freetype2`
LIBS = -lglfw -lGLEW -lGL -lX11 -lXi -L./build
INCLUDE = -I./include `pkg-config --cflags freetype2`

SRC_DIR = ./src
SRC = $(wildcard $(SRC_DIR)/*.cpp)

BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/lib$(TARGET_NAME).a

OBJ = $(addprefix $(BUILD_DIR)/obj/, $(notdir $(SRC:.cpp=.o)))
DEPEND = $(OBJ:.o=.d)

TEST_SRC_DIR = ./tests
TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_BUILD_DIR = $(BUILD_DIR)/tests
TEST_TARGET = $(TEST_BUILD_DIR)/$(basename $(notdir $(TEST_SRC)))
TEST_OBJ = $(addprefix $(TEST_BUILD_DIR)/obj/, $(notdir $(TEST_SRC:.cpp=.o)))
TEST_DEPEND = $(TEST_OBJ:.o=.d)

EXAMPLE_SRC_DIR = ./examples
EXAMPLE_SRC = $(wildcard $(EXAMPLE_SRC_DIR)/*.cpp)
EXAMPLE_BUILD_DIR = $(BUILD_DIR)/examples
EXAMPLE_TARGET = $(EXAMPLE_BUILD_DIR)/$(basename $(notdir $(EXAMPLE_SRC)))
EXAMPLE_OBJ = $(addprefix $(EXAMPLE_BUILD_DIR)/obj/, $(notdir $(EXAMPLE_SRC:.cpp=.o)))
EXAMPLE_DEPEND = $(EXAMPLE_OBJ:.o=.d)

GENERATED = $(OBJ) $(DEPEND) $(TARGET) \
	$(TEST_OBJ) $(TEST_DEPEND) $(TEST_TARGET)  \
	$(EXAMPLE_OBJ) $(EXAMPLE_DEPEND) $(EXAMPLE_TARGET)  \

.PHONY: all
all: test example

.PHONY: debug
debug: CXXFLAGS+=$(DEBUG_CXXFLAGS)
debug: $(TARGET)

.PHONY: release
release: CXXFLAGS+=$(RELEASE_CXXFLAGS)
release: $(TARGET)

.PHONY: test-build
test-build: CXXFLAGS+=$(TEST_CXX_FLAGS)
test-build: $(TEST_TARGET)

.PHONY: test
test: test-build
	$(TEST_TARGET)

.PHONY: example
example: CXXFLAGS+=$(EXAMPLE_CXX_FLAGS)
example: $(EXAMPLE_TARGET)

-include $(DEPEND)
-include $(TEST_DEPEND)
-include $(EXAMPLE_DEPEND)

$(TARGET): $(OBJ)
	$(AR) $(AR_FLAGS) -o $@ $^

$(TEST_TARGET): $(TARGET) $(TEST_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS) -l$(TARGET_NAME)

$(EXAMPLE_TARGET): $(TARGET) $(EXAMPLE_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS) -l$(TARGET_NAME)

$(BUILD_DIR)/obj/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c -MMD -MP $<

$(TEST_BUILD_DIR)/obj/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c -MMD -MP $<

$(EXAMPLE_BUILD_DIR)/obj/%.o: $(EXAMPLE_SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c -MMD -MP $<

.PHONY: clean
clean:
	-rm -f $(OBJ) $(DEPEND) $(TARGET) $(TEST_OBJ) $(TEST_DEPEND) $(TEST_TARGET) 
