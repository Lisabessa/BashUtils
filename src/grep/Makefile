CC = gcc
FLAGS = -Wall -Werror -Wextra

all: realized_grep

realized_grep: realized_grep.c
	$(CC) $(FLAGS) realized_grep.c -o realized_grep
	make clean

clean:
	rm -rf *.o