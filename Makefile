GL_PKGS=glfw3
CFLAGS=-Wall -Wextra
SRC=main.c glad/src/glad.c

main: main.c
	$(CC) $(CFLAGS) `pkg-config --cflags $(GL_PKGS)` -o main $(SRC) `pkg-config --libs $(GL_PKGS)` -ldl -lX11 -lm -lpthread

