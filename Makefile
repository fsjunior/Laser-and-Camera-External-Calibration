all:
		gcc -c camera.c `pkg-config --cflags playerc gsl` -Wall
		gcc -c homography.c `pkg-config --cflags playerc gsl` -Wall
		gcc -c laser.c `pkg-config --cflags playerc gsl` -Wall
		gcc -c utils.c `pkg-config --cflags playerc gsl` -Wall
		gcc -c main.c `pkg-config --cflags playerc gsl` -Wall
		gcc -o cone_detector camera.o homography.o main.o laser.o utils.o `pkg-config --libs playerc gsl` -lm -Wall
		

clean:
		rm *.o cone-detector
