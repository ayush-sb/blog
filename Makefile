CC = gcc
DIR = src

blogmake: $(DIR)/blog.c $(DIR)/utils.c
	$(CC) -o blog $(DIR)/blog.c $(DIR)/utils.c

.PHONY: clean
clean:
	rm -f blog $(DIR)/*.o
