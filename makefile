all:run

run:plotter.c
	gcc -o run plotter.c -lncurses -std=c99
clean:
	rm run
