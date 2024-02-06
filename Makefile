CC = gcc
DIR = src
CFLAGS = -I./$(DIR)
DEPS = $(DIR)/blog.h


$(DIR)/%.o: ($DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

blogmake: $(DIR)/blog.o
	$(CC) -o blog $(DIR)/blog.o

.PHONY: clean
clean:
	rm -f blog $(DIR)/*.o
