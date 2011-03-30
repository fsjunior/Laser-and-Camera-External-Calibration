/* 
 * File:   homography.h
 * Author: fsjunior
 *
 * Created on March 29, 2011, 2:43 PM
 */

#ifndef _HOMOGRAPHY_H
#define	_HOMOGRAPHY_H

#include <gsl/gsl_matrix.h>
#include "utils.h"


#ifdef	__cplusplus
extern "C" {
#endif


    typedef struct {
        gsl_matrix *homography_matrix;
        gsl_matrix *base_points;
        gsl_matrix *world_points;
        int num_points;
        int max_points;
    } homography_ctx;

    void init_homography(homography_ctx *ctx, int max_points);

    void free_homography(homography_ctx *ctx);

    int homography_add_points(homography_ctx *ctx, cone_laser_pos *laser_pos, cone_camera_pos *camera_pos);

    void compute_homography(homography_ctx *ctx);

    void image2world(homography_ctx *ctx, int u, int v, double zpos, double *xpos, double *ypos);

    void world2image(homography_ctx *ctx, double xpos, double ypos, double zpos, int *u, int *v);



#ifdef	__cplusplus
}
#endif

#endif	/* _HOMOGRAPHY_H */

