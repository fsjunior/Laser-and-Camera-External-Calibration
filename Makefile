all:
		gcc -c camera.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c homography.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c laser.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c utils.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c imagemask.c `pkg-config --cflags playerc gsl opencv` -Wall
		gcc -c classifier.c `pkg-config --cflags playerc gsl opencv fann` -Wall
		gcc -c main.c `pkg-config --cflags playerc gsl opencv fann` -Wall
		gcc -o cone_detector  classifier.o homography.o main.o laser.o utils.o camera.o imagemask.o `pkg-config --libs playerc gsl opencv fann` -lm -Wall

clean:
		rm *.o cone_detector
