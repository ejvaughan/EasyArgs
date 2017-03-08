CC = gcc
PROJECT_SRC_DIR = ..
BUILD_DIR = build
CFLAGS = -I$(PROJECT_SRC_DIR)
CFLAGS += -I$(PROJECT_SRC_DIR)/uthash/include

DEPS = $(PROJECT_SRC_DIR)/EasyArgs.h
_OBJ = tests.o EasyArgs.o
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ))

$(BUILD_DIR)/%.o: $(PROJECT_SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

tests: $(OBJ)
	gcc -o $(BUILD_DIR)/$@ $^ $(CFLAGS)
