all:run

run:plotter.c
	gcc -o run plotter.c -lncurses
clean:
	rm run
