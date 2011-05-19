#include <stdio.h>
#include <libplayerc/playerc.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "laser.h"
#include "camera.h"
#include "homography.h"
#include "imagemask.h"
//#include <gsl/gsl_matrix.h>

#define MAX_POINTS 16

// centimetros

typedef enum {
    CALIBRATION, COMPUTED
} CALIBRATION_STATE;

void wait_snapshot(int event, int x, int y, int flags, void* param)
{
    int *snapshot = (int *) param;
    *snapshot = event == CV_EVENT_LBUTTONDOWN;
}

int main(const int argc, char *argv[])
{
    playerc_client_t *client;
    playerc_laser_t *laser;
    playerc_camera_t *camera;
    cone_laser_detector_ctx cone_laser_detector;
    cone_camera_detector_ctx cone_camera_detector;
    homography_ctx homography;
    int snapshot = 0;
    CALIBRATION_STATE calibration_state = CALIBRATION;
    image_mask_ctx image_mask;


    int u, v;
    int u2, v2;
    int i;
    double x, y;


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
    init_cone_camera_undistortion_matrix(&cone_camera_detector, "Intrinsics.xml", "Distortion.xml");
    init_homography(&homography, MAX_POINTS);
    
    init_image_mask(&image_mask, &homography, laser, 0.29);

    if(argc == 2) {
        load_homography(&homography, argv[1]);
        calibration_state = COMPUTED;
    }

    do {
        if(!playerc_client_read(client))
            return -1;

        copy_cone_camera_image(&cone_camera_detector);

        switch(calibration_state) {
            case CALIBRATION: //CALIBRAĆÃO
                cvSetMouseCallback("Calibration", wait_snapshot, &snapshot);

                // adesão de pontos
                if(snapshot == 1) {
                    calc_cone_laser_pos(&cone_laser_detector);
                    calc_cone_camera_pos(&cone_camera_detector, &cone_laser_detector);

                    snapshot = 0;
                    homography_add_points(&homography, &(cone_laser_detector.pos), &(cone_camera_detector.pos));
                    
                    if(homography.num_points >= homography.max_points) {
                        compute_homography(&homography);
                        save_homography(&homography, "homography.dat");
                        calibration_state = COMPUTED;
                    }
                }
                break;

            case COMPUTED: // homografia computada
                compute_mask(&image_mask);
                cvShowImage("Image Mask", image_mask.image);
                

                for(i = 0; i < cone_laser_detector.laser->scan_count; i++) {
                    calc_x_y(cone_laser_detector.laser->scan_count, i, cone_laser_detector.laser->ranges[i], &x, &y);
                    world2image(&homography, x, y, 0.29, &u, &v);
                    cvCircle(cone_camera_detector.image, cvPoint(u, v), 2, CV_RGB(0, 255, 0), -1, 8, 0);

                    //if(cone_laser_detector.laser->ranges[i] < 3.0) {
                    world2image(&homography, x, y, 0, &u2, &v2);
                    cvLine(cone_camera_detector.image, cvPoint(u, v), cvPoint(u2, v2), CV_RGB(0, 0, 255), 1, 8, 0);
                    //}
                }

                break;
        }

        cvShowImage("Calibration", cone_camera_detector.image);
    } while((char)cvWaitKey(10) != 27);


    free_cone_camera_detector_ctx(&cone_camera_detector);
    free_homography(&homography);
    free_image_mask(&image_mask);

    playerc_camera_unsubscribe(camera);
    playerc_camera_destroy(camera);
    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 1;
}
