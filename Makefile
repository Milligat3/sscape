CC := gcc
TARGET := sscapes.exe
RAYLIB_PATH := D:/ProgrStuff/raylib/raylib/src
LDFLAGS := -LD:/ProgrStuff/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
CFLAGS := -ID:/ProgrStuff/raylib/raylib/src -std=c99 -Wall 


all: $(TARGET)

$(TARGET): main.o
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo [SUCCESS] $(TARGET) built!

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf /Q $(TARGET) *.o
