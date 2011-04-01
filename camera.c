
#include "camera.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <unistd.h>

/* declaration */
void defineCalibrationPoints(int event, int x, int y, int flags, void* param);

/* definition */

void init_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_detector, playerc_camera_t *camera)
{
    cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
    cone_detector->image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    cone_detector->camera = camera;
}

void calc_cone_camera_pos(cone_camera_detector_ctx *cone_detector, cone_laser_detector_ctx *cone_laser_detector)
{
    long i;


    //memcpy((unsigned char*)image->imageData, (unsigned char*), cone_detector->camera->image_count);

    /* Inverte os canais de cores da imagem.
     * */
    printf("z:%f\n", cone_laser_detector->pos.z);
    fflush(stdout);
    
    for(i = 0; i < cone_detector->image->width *  cone_detector->image->height; i++) {
        cone_detector->image->imageData[3 * i] = cone_detector->camera->image[3 * i + 2];
        cone_detector->image->imageData[3 * i + 1] = cone_detector->camera->image[3 * i + 1];
        cone_detector->image->imageData[3 * i + 2] = cone_detector->camera->image[3 * i];
    }

    
    cone_detector->num_points = 0;
    cvSetMouseCallback("Calibration", defineCalibrationPoints, cone_detector);

    while(cvWaitKey(30) != 27 && cone_detector->num_points < 2) {
        cvCircle(cone_detector->image, cvPoint(cone_detector->pos.x1, cone_detector->pos.y1), 2, CV_RGB(0, 255, 0), -1, 8, 0);
        cvCircle(cone_detector->image, cvPoint(cone_detector->pos.x2, cone_detector->pos.y2), 2, CV_RGB(0, 0, 255), -1, 8, 0);
        cvShowImage("Calibration", cone_detector->image);
    }


    
}

void defineCalibrationPoints(int event, int x, int y, int flags, void* param)
{
    cone_camera_detector_ctx *cone_detector = (cone_camera_detector_ctx *) param;
    switch(event) {
        case CV_EVENT_LBUTTONDOWN:
            if(cone_detector->num_points == 0) {
                cone_detector->pos.x1 = x;
                cone_detector->pos.y1 = y;
            } else {
                cone_detector->pos.x2 = x;
                cone_detector->pos.y2 = y;
            }
            cone_detector->num_points++;
            break;
    }
}

void free_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_detector)
{
    cvReleaseImage(&(cone_detector->image));

}