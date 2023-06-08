CC = gcc
FLAGS = -Wall -Werror -Wextra

all: realized_cat

realized_cat: realized_cat.c
	$(CC) $(FLAGS) realized_cat.c -o realized_cat
	make clean

clean:
	rm -rf *.o