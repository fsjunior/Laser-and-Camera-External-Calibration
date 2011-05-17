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
    cone_camera_pos pos;
    int num_points;
} cone_camera_detector_ctx;

void init_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_camera_detector, playerc_camera_t *camera);
void init_cone_camera_undistortion_matrix(cone_camera_detector_ctx *cone_camera_detector, char *intrinsics, char *distortion);
void calc_cone_camera_pos(cone_camera_detector_ctx *cone_camera_detector, cone_laser_detector_ctx *cone_laser_detector);
void copy_cone_camera_image(cone_camera_detector_ctx *cone_camera_detector);
void free_cone_camera_detector_ctx(cone_camera_detector_ctx *cone_camera_detector);

#endif	/* _CAMERA_H */

