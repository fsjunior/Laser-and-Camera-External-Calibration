/*
 * File:   main.c
 * Author: Francisco de Souza Júnior
 *
 * Created on March 29, 2011, 2:43 PM
 */

#include <stdio.h>
#include <libplayerc/playerc.h>
#include <string.h>
#include "laser.h"
#include "camera.h"
#include "homography.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include <gsl/gsl_matrix.h>

#define MAX_HOMOGRAPHY_POINTS 10


// centimetros

void wait_snapshot(int event, int x, int y, int flags, void* param)
{
    int *snapshot = (int *) param;

    *snapshot = event == CV_EVENT_LBUTTONDOWN;
}

int main(int argc, char *argv[])
{
    playerc_client_t *client;
    playerc_laser_t *laser;
    playerc_camera_t *camera;
    cone_laser_detector_ctx cone_laser_detector;
    cone_camera_detector_ctx cone_camera_detector;
    homography_ctx homography;
    int snapshot = 0;
    long i;


    // Create a client and connect it to the server.
    client = playerc_client_create(NULL, "localhost", 6665);
    if(0 != playerc_client_connect(client))
        return -1;


    laser = playerc_laser_create(client, 0);
    if(playerc_laser_subscribe(laser, PLAYER_OPEN_MODE))
        return -2;

    camera = playerc_camera_create(client, 0);
    if(playerc_camera_subscribe(camera, PLAYER_OPEN_MODE))
        return -3;


    init_cone_laser_detector_ctx(&cone_laser_detector, laser);
    init_cone_camera_detector_ctx(&cone_camera_detector, camera);
    cvSetMouseCallback("Calibration", wait_snapshot, &snapshot);
    init_homography(&homography, MAX_HOMOGRAPHY_POINTS);

    do {
        if(!playerc_client_read(client))
            return -1;


        for(i = 0; i < cone_camera_detector.image->width * cone_camera_detector.image->height; i++) {
            cone_camera_detector.image->imageData[3 * i] = cone_camera_detector.camera->image[3 * i + 2];
            cone_camera_detector.image->imageData[3 * i + 1] = cone_camera_detector.camera->image[3 * i + 1];
            cone_camera_detector.image->imageData[3 * i + 2] = cone_camera_detector.camera->image[3 * i];
        }

        cvShowImage("Calibration", cone_camera_detector.image);
        cvWaitKey(10);

        if(snapshot) {
            calc_cone_laser_pos(&cone_laser_detector);
            calc_cone_camera_pos(&cone_camera_detector, &cone_laser_detector);
            cvSetMouseCallback("Calibration", wait_snapshot, &snapshot);
            printf("x1:%f y1:%f x2:%f y2:%f z:%f\n", cone_laser_detector.pos.x1, cone_laser_detector.pos.y1, cone_laser_detector.pos.x2, cone_laser_detector.pos.y2, cone_laser_detector.pos.z);
            printf("x1:%d y1:%d x2:%d y2:%d", cone_camera_detector.pos.x1, cone_camera_detector.pos.y1, cone_camera_detector.pos.x2, cone_camera_detector.pos.y2);
            fflush(stdout);
            snapshot = 0;
        }


        //printf("x1:%f y1:%f x2:%f y2:%f z:%f\n", cone_laser_detector.pos.x1, cone_laser_detector.pos.y1, cone_laser_detector.pos.x2, cone_laser_detector.pos.y2, cone_laser_detector.pos.z);
        //fflush(stdout);
    } while(1);

    free_homography(&homography);
    free_cone_camera_detector_ctx(&cone_camera_detector);

    playerc_camera_unsubscribe(camera);
    playerc_camera_destroy(camera);
    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 0;
}
