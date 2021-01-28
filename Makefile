all: build

build::
	gcc -g -o server src/main.c

clean:
	rm -f server
