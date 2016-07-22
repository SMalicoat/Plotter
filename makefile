all:run

run:plotter.c
	gcc -o run plotter.c -lncurses -lwiringPi -lpthread  
clean:
	rm run
