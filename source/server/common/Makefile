SRC = $(wildcard *.c)
TARGET = $(patsubst %.c, %.o, $(SRC))

all:$(TARGET)

$(TARGET):%.o:%.c
	$(CC) -c $< -o $(OUTPUT_DIR)/$@ $(CFLAGS)

#MOVE_OBJ:
#	$(foreach obj, $(TARGET), $(shell cp $(obj) $(OUTPUT_DIR)))

.PHONY:clean
clean:
	$(RM) -r $(TARGET)