

run: build
	./main

build:
	gcc -g main.c -o main

val: build
	valgrind --leak-check=full --track-origins=yes --log-file=val.log ./main
