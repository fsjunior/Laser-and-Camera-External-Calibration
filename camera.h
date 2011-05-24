/* 
 * File:   camera.h
 * Author: Francisco de Souza Júnior
 *
 * Created on March 30, 2011, 2:07 PM
 */

#ifndef _CAMERA_H
#define	_CAMERA_H

#include "utils.h"
#include "laser.h"
#include <libplayerc/playerc.h>
#include "opencv/cv.h"

typedef struct {
    playerc_camera_t *camera;
    IplImage *image;
    IplImage *mapx, *mapy;
    int undistortion;
    obstacle_camera_pos pos;
    int num_points;
} obstacle_camera_detector_ctx;

void init_obstacle_camera_detector_ctx(obstacle_camera_detector_ctx *obstacle_camera_detector, playerc_camera_t *camera);
void init_obstacle_camera_undistortion_matrix(obstacle_camera_detector_ctx *obstacle_camera_detector, char *intrinsics, char *distortion);
void calc_obstacle_camera_pos(obstacle_camera_detector_ctx *obstacle_camera_detector, obstacle_laser_detector_ctx *obstacle_laser_detector);
void copy_obstacle_camera_image(obstacle_camera_detector_ctx *obstacle_camera_detector);
void free_obstacle_camera_detector_ctx(obstacle_camera_detector_ctx *obstacle_camera_detector);

#endif	/* _CAMERA_H */

