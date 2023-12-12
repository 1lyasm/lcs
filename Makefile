all:
	clang -g -fsanitize=address -ansi \
		-Wall src/main.c -o main
run:
	./main
