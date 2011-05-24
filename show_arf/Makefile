all:
		gcc -c main.c `pkg-config --cflags playerc gsl opencv fann` -Wall
		gcc -o show_arff  main.o `pkg-config --libs playerc gsl opencv fann` -lm -Wall

clean:
		rm *.o show_arff
