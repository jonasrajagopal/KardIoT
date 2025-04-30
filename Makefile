all:
	gcc -o data_plot data_plot.c -lSDL2
	gcc -o data_plot_in data_plot_in.c -lSDL2
	gcc -o data_read data_read.c -lSDL2

clean:
	rm data_plot data_plot_in data_read
#	./data_read /dev/ttyUSB1 |  ./data_plot_in /tmp/tmp