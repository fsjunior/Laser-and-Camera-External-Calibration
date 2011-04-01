all:
		gcc -c camera.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c homography.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c laser.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c utils.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c main.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -o cone_detector  homography.o main.o laser.o utils.o camera.o `pkg-config --libs playerc gsl opencv` -lm -Wall
		

clean:
		rm *.o cone-detector
