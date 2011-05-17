
#include "camera.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <unistd.h>

/* declaration */
void defineCalibrationPoints(int event, int x, int y, int flags, void* param);

/* definition */

void init_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_camera_detector, playerc_camera_t *camera)
{
    cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
    cone_camera_detector->image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    cone_camera_detector->camera = camera;
    cone_camera_detector->undistortion = 0;
}

void init_cone_camera_undistortion_matrix(cone_camera_detector_ctx *cone_camera_detector, char *intrinsics, char *distortion)
{
    CvMat *i = (CvMat*) cvLoad(intrinsics, NULL, NULL, NULL);
    CvMat *d = (CvMat*) cvLoad(distortion, NULL, NULL, NULL);

    cone_camera_detector->mapx = cvCreateImage(cvGetSize(cone_camera_detector->image), IPL_DEPTH_32F, 1);
    cone_camera_detector->mapy = cvCreateImage(cvGetSize(cone_camera_detector->image), IPL_DEPTH_32F, 1);


    cvInitUndistortMap(i, d, cone_camera_detector->mapx, cone_camera_detector->mapy);

    cone_camera_detector->undistortion = 1;
    cvReleaseMat(&i);
    cvReleaseMat(&d);
    //free intrinsic distortion
}

void copy_cone_camera_image(cone_camera_detector_ctx *cone_camera_detector)
{
    int i;
    IplImage *image;
/*
    printf("asd %d %d", cone_camera_detector->image->width, cone_camera_detector->image->height);
    fflush(stdout);
    int a = cone_camera_detector->camera->image[cone_camera_detector->image->width * cone_camera_detector->image->height * 3];
    printf("asd\n");
    fflush(stdout);
*/

    for(i = 0; i < cone_camera_detector->image->width * cone_camera_detector->image->height; i++) {
        cone_camera_detector->image->imageData[3 * i] = cone_camera_detector->camera->image[3 * i + 2];
        cone_camera_detector->image->imageData[3 * i + 1] = cone_camera_detector->camera->image[3 * i + 1];
        cone_camera_detector->image->imageData[3 * i + 2] = cone_camera_detector->camera->image[3 * i];
    }



    if(cone_camera_detector->undistortion) {
        image = cvCloneImage(cone_camera_detector->image);
        cvRemap(image, cone_camera_detector->image, cone_camera_detector->mapx, cone_camera_detector->mapy, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
        cvReleaseImage(&image);
    }
}

void calc_cone_camera_pos(cone_camera_detector_ctx *cone_camera_detector, cone_laser_detector_ctx *cone_laser_detector)
{
    int i;

    //memcpy((unsigned char*)image->imageData, (unsigned char*), cone_camera_detector->camera->image_count);

    /* Inverte os canais de cores da imagem.
     * */
    printf("Z p/ 84cm e Z para 0.39cm\n");
    printf("Dist: %f\n", calc_dist(&(cone_laser_detector->pos)));

    fflush(stdout);
   
    
    copy_cone_camera_image(cone_camera_detector);

    
    cone_camera_detector->num_points = 0;
    cvSetMouseCallback("Calibration", defineCalibrationPoints, cone_camera_detector);

    

    while(cone_camera_detector->num_points < 4 && cvWaitKey(10) != 27) {
        for(i = 0; i < cone_camera_detector->num_points; i++)
            cvCircle(cone_camera_detector->image, cvPoint(cone_camera_detector->pos.x[i], cone_camera_detector->pos.y[i]), 2, CV_RGB(0, 255, 0), -1, 8, 0);
        cvShowImage("Calibration", cone_camera_detector->image);
    }


    printf("asd");
    fflush(stdout);

}

void defineCalibrationPoints(int event, int x, int y, int flags, void* param)
{
    cone_camera_detector_ctx *cone_camera_detector = (cone_camera_detector_ctx *) param;
    switch(event) {
        case CV_EVENT_LBUTTONDOWN:
            cone_camera_detector->pos.x[cone_camera_detector->num_points] = x;
            cone_camera_detector->pos.y[cone_camera_detector->num_points] = y;
            cone_camera_detector->num_points++;
            break;
    }
}

void free_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_camera_detector)
{
    cvReleaseImage(&(cone_camera_detector->image));
    
    if(cone_camera_detector->undistortion) {
        cvReleaseImage(&(cone_camera_detector->mapx));
        cvReleaseImage(&(cone_camera_detector->mapy));
    }
}