
#include "camera.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <unistd.h>

/* declaration */
void defineCalibrationPoints(int event, int x, int y, int flags, void* param);

/* definition */

void init_obstacle_camera_detector_ctx(obstacle_camera_detector_ctx *obstacle_camera_detector, playerc_camera_t *camera)
{
    cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
    obstacle_camera_detector->image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    obstacle_camera_detector->camera = camera;
    obstacle_camera_detector->undistortion = 0;
}

void init_obstacle_camera_undistortion_matrix(obstacle_camera_detector_ctx *obstacle_camera_detector, char *intrinsics, char *distortion)
{
    CvMat *i = (CvMat*) cvLoad(intrinsics, NULL, NULL, NULL);
    CvMat *d = (CvMat*) cvLoad(distortion, NULL, NULL, NULL);

    obstacle_camera_detector->mapx = cvCreateImage(cvGetSize(obstacle_camera_detector->image), IPL_DEPTH_32F, 1);
    obstacle_camera_detector->mapy = cvCreateImage(cvGetSize(obstacle_camera_detector->image), IPL_DEPTH_32F, 1);


    cvInitUndistortMap(i, d, obstacle_camera_detector->mapx, obstacle_camera_detector->mapy);

    obstacle_camera_detector->undistortion = 1;
    cvReleaseMat(&i);
    cvReleaseMat(&d);
    //free intrinsic distortion
}

void copy_obstacle_camera_image(obstacle_camera_detector_ctx *obstacle_camera_detector)
{
    int i;
    IplImage *image;

    for(i = 0; i < obstacle_camera_detector->image->width * obstacle_camera_detector->image->height; i++) {
        obstacle_camera_detector->image->imageData[3 * i] = obstacle_camera_detector->camera->image[3 * i + 2];
        obstacle_camera_detector->image->imageData[3 * i + 1] = obstacle_camera_detector->camera->image[3 * i + 1];
        obstacle_camera_detector->image->imageData[3 * i + 2] = obstacle_camera_detector->camera->image[3 * i];
    }

    if(obstacle_camera_detector->undistortion) {
        image = cvCloneImage(obstacle_camera_detector->image);
        cvRemap(image, obstacle_camera_detector->image, obstacle_camera_detector->mapx, obstacle_camera_detector->mapy, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
        cvReleaseImage(&image);
    }
}

void calc_obstacle_camera_pos(obstacle_camera_detector_ctx *obstacle_camera_detector, obstacle_laser_detector_ctx *obstacle_laser_detector)
{
    int i;

    /* Inverte os canais de cores da imagem.
     * */
    //printf("Z p/ 84cm e Z para 0.39cm\n");
    printf("Range: %f\n", obstacle_laser_detector->laser->ranges[obstacle_laser_detector->minor_scan_index]);

    fflush(stdout);

    copy_obstacle_camera_image(obstacle_camera_detector);

    obstacle_camera_detector->num_points = 0;
    cvSetMouseCallback("Calibration", defineCalibrationPoints, obstacle_camera_detector);

    while(obstacle_camera_detector->num_points < 2 && cvWaitKey(10) != 27) {
        for(i = 0; i < obstacle_camera_detector->num_points; i++)
            cvCircle(obstacle_camera_detector->image, cvPoint(obstacle_camera_detector->pos.x[i], obstacle_camera_detector->pos.y[i]), 2, CV_RGB(0, 255, 0), -1, 8, 0);
        cvShowImage("Calibration", obstacle_camera_detector->image);
    }
}

void defineCalibrationPoints(int event, int x, int y, int flags, void* param)
{
    obstacle_camera_detector_ctx *obstacle_camera_detector = (obstacle_camera_detector_ctx *) param;
    switch(event) {
        case CV_EVENT_LBUTTONDOWN:
            obstacle_camera_detector->pos.x[obstacle_camera_detector->num_points] = x;
            obstacle_camera_detector->pos.y[obstacle_camera_detector->num_points] = y;
            obstacle_camera_detector->num_points++;
            break;
    }
}

void free_obstacle_camera_detector_ctx(obstacle_camera_detector_ctx *obstacle_camera_detector)
{
    cvReleaseImage(&(obstacle_camera_detector->image));

    if(obstacle_camera_detector->undistortion) {
        cvReleaseImage(&(obstacle_camera_detector->mapx));
        cvReleaseImage(&(obstacle_camera_detector->mapy));
    }
}