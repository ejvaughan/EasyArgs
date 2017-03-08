CC = gcc
PROJECT_SRC_DIR = ..
BUILD_DIR = build
CFLAGS = -I$(PROJECT_SRC_DIR)
CFLAGS += -I$(PROJECT_SRC_DIR)/uthash/include
CFLAGS += -g

DEPS = $(PROJECT_SRC_DIR)/EasyArgs.h

CONF = $(wildcard *.conf)
BUILD_CONF = $(patsubst %.conf, $(BUILD_DIR)/%.conf, $(CONF))

OBJ = $(patsubst $(PROJECT_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(PROJECT_SRC_DIR)/*.c))
OBJ += $(patsubst %.c, $(BUILD_DIR)/%.o, $(wildcard *.c))

$(BUILD_DIR)/tests: $(OBJ) $(BUILD_CONF)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(BUILD_DIR)/%.conf: %.conf
	cp -f $< $@

$(BUILD_DIR)/%.o: $(PROJECT_SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)


