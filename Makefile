build:
	gcc -Wall -std=c99 -lsdl2 ./src/*.c -o renderer

run:
	./renderer

clean:
	rm ./renderer
