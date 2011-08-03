/* 
 * File:   imagemask.h
 * Author: Francisco de Souza Júnior
 *
 * Created on May 17, 2011, 10:11 AM
 */

#ifndef IMAGEMASK_H
#define	IMAGEMASK_H

#include "homography.h"
#include "laser.h"
#include "camera.h"
#include "opencv/cv.h"

//red
#define NON_NAVIGABLE_REGION CV_RGB(255, 0, 0)
//green
#define NAVIGABLE_REGION CV_RGB(0, 255, 0)
//blue
#define BORDER_REGION CV_RGB(0, 0, 255)



#define HALF_LINE_OBSTACLE_WIDTH 3
#define HALF_LINE_BORDER_WIDTH 15
#define HALF_LINE_NAVIGABLE_WIDTH HALF_LINE_BORDER_WIDTH

#define MAX_LASER_READ_SIZE 8.0

#define BLOCK_SIZE 10

typedef struct {
    IplImage *image;
    homography_ctx *homography;
    obstacle_camera_detector_ctx *camera;
    playerc_laser_t *laser;
    double lazer_z_index;
} image_mask_ctx;

void init_image_mask(image_mask_ctx *ctx, homography_ctx *homography, obstacle_camera_detector_ctx *camera, playerc_laser_t *laser, double laser_z_index);

void compute_mask(image_mask_ctx *ctx);

void save_mask_as_arff(image_mask_ctx *ctx, char *filename);

void free_image_mask(image_mask_ctx *ctx);


#endif	/* IMAGEMASK_H */

