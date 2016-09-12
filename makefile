all:run

run:plotter.v3.c
	gcc -o run plotter.v3.c -lncurses -lwiringPi -lpthread  
clean:
	rm run
