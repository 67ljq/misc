#测试文件的Makefile

SRC = $(wildcard *.c)
TARGET = $(patsubst %.c,%.o, $(SRC))

all:$(TARGET)

%.o:%.c
	$(CC) -c $< -o $(OUTPUT_DIR)/$@ $(CFLAGS)

.PHONY:clean
clean:
	$(RM) -r $(TARGET)