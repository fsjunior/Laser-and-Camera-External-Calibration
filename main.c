#include <stdio.h>
#include <libplayerc/playerc.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "laser.h"
#include "camera.h"
#include "homography.h"
//#include <gsl/gsl_matrix.h>

#define MAX_POINTS 10

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
    int snapshot = 0, i;

    // Create a client and connect it to the server.
    client = playerc_client_create(NULL, "localhost", 6665);
    if(0 != playerc_client_connect(client))
        return -1;

    laser = playerc_laser_create(client, 0);
    if(playerc_laser_subscribe(laser, PLAYER_OPEN_MODE))
        return -1;

    camera = playerc_camera_create(client, 0);
    if(playerc_camera_subscribe(camera, PLAYER_OPEN_MODE))
        return -1;


    init_cone_laser_detector_ctx(&cone_laser_detector, laser);
    init_cone_camera_detector_ctx(&cone_camera_detector, camera);
    init_homography(&homography, MAX_POINTS);

    do {
        if(!playerc_client_read(client))
            return -1;

        for(i = 0; i < cone_camera_detector.image->width * cone_camera_detector.image->height; i++) {
            cone_camera_detector.image->imageData[3 * i] = cone_camera_detector.camera->image[3 * i + 2];
            cone_camera_detector.image->imageData[3 * i + 1] = cone_camera_detector.camera->image[3 * i + 1];
            cone_camera_detector.image->imageData[3 * i + 2] = cone_camera_detector.camera->image[3 * i];
        }

        /* homografia computada */
        if(homography.num_points >= homography.max_points) {
            int u, v;
            calc_cone_laser_pos(&cone_laser_detector);

            world2image(&homography, cone_laser_detector.pos.x1, cone_laser_detector.pos.y1, cone_laser_detector.pos.z, &u, &v);
            cvCircle(cone_camera_detector.image, cvPoint(v, u), 2, CV_RGB(0, 255, 0), -1, 8, 0);

            world2image(&homography, cone_laser_detector.pos.x2, cone_laser_detector.pos.x2, cone_laser_detector.pos.z, &u, &v);
            cvCircle(cone_camera_detector.image, cvPoint(v, u), 2, CV_RGB(0, 0, 255), -1, 8, 0);

        }

        cvSetMouseCallback("Calibration", wait_snapshot, &snapshot);
        cvShowImage("Calibration", cone_camera_detector.image);
        cvWaitKey(10);


        /* adesão de pontos */
        if(snapshot == 1 && homography.num_points < homography.max_points) {
            calc_cone_laser_pos(&cone_laser_detector);
            calc_cone_camera_pos(&cone_camera_detector, &cone_laser_detector);
            snapshot = 0;
            printf("Laser: x1:%f y1:%f x2:%f y2:%f z:%f\n", cone_laser_detector.pos.x1, cone_laser_detector.pos.y1, cone_laser_detector.pos.x2, cone_laser_detector.pos.y2, cone_laser_detector.pos.z);
            printf("Camera: x1:%d y1:%d x2:%d y2:%d\n", cone_camera_detector.pos.x1, cone_camera_detector.pos.y1, cone_camera_detector.pos.x2, cone_camera_detector.pos.y2);
            homography_add_points(&homography, &(cone_laser_detector.pos), &(cone_camera_detector.pos));
            fflush(stdout);
            if(homography.num_points >= homography.max_points)
                compute_homography(&homography);
        }




        //printf("x1:%f y1:%f x2:%f y2:%f z:%f\n", cone_detector.pos.x1, cone_detector.pos.y1, cone_detector.pos.x2, cone_detector.pos.y2, cone_detector.pos.z);
        //fflush(stdout);
    } while(1);

    free_cone_camera_detector_ctx(&cone_camera_detector);

    playerc_camera_unsubscribe(camera);
    playerc_camera_destroy(camera);
    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 1;
}
