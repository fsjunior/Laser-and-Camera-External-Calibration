/* 
 * File:   imagemask.h
 * Author: fsjunior
 *
 * Created on May 17, 2011, 10:11 AM
 */

#ifndef IMAGEMASK_H
#define	IMAGEMASK_H

#include "homography.h"
#include "laser.h"

//red
#define NON_NAVIGABLE_REGION CV_RGB(255, 0, 0)
//green
#define NAVIGABLE_REGION CV_RGB(0, 255, 0)
//blue
#define BORDER_REGION CV_RGB(0, 0, 255)

#define HALF_LINE_OBSTACLE_WIDTH 3
#define HALF_LINE_BORDER_WIDTH 15

#define MAX_LASER_READ_SIZE 1.5

typedef struct {
    IplImage *image;
    homography_ctx *homography;
    playerc_laser_t *laser;
    double lazer_z_index;
} image_mask_ctx;

void init_image_mask(image_mask_ctx *ctx, homography_ctx *homography, playerc_laser_t *laser, double laser_z_index);

void compute_mask(image_mask_ctx *ctx);

void free_image_mask(image_mask_ctx *ctx);


#endif	/* IMAGEMASK_H */

