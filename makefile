# Makefile para Pac-Man com múltiplos arquivos
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
LDFLAGS = -lraylib -lm -lopengl32 -lgdi32 -lwinmm  # Windows
# LDFLAGS = -lraylib -lm  # Linux/macOS
TARGET = pacman.exe
SOURCES = main.c mapa.c entidades.c colisoes.c jogo.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)