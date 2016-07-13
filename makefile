all:run

run:plotter.c
	gcc -o run plotter.c
clean:
	rm run
