all:run

run:plotter.c
	gcc -o run plotter.c -lncurses -lwiringPi -lpthread ~/wiringPi/wiringPi/softServo.c -std=c99
clean:
	rm run
