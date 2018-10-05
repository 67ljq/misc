SRC = $(wildcard ./src/*.c)
TARGET := $(subst */,, $(SRC))
TARGET = $(patsubst %.c, %.o, $(SRC))

all:$(TARGET) MOVE_OBJ

$(TARGET):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)

MOVE_OBJ:
	$(foreach obj, $(TARGET), $(shell cp $(obj) $(OUTPUT_DIR)))

.PHONY:clean
clean:
	$(RM) -r $(TARGET)