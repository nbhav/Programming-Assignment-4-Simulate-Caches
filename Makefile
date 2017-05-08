all:first

first: first.c
	gcc -Wall -Werror -lm -std=c99 -o3  -fsanitize=address first.c -o first

clean:
	rm -rf first 
