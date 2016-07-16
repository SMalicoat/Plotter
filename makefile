all:run

run:plotter.c
	gcc -o run plotter.c -lncurses -lwiringPi -lpthread -I~/wiringPi/wiringPi/ -std=c99
clean:
	rm run
