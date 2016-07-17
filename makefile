all:run

run:plotter.c
	gcc -o run plotter.c -lncurses -lwiringPi -lpthread  -std=c99
clean:
	rm run
