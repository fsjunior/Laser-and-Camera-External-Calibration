/*
 * File:   homography.c
 * Author: Francisco de Souza Júnior and Patrick Shinzato
 *
 * Created on March 29, 2011, 2:43 PM
 */


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <math.h>

#include "homography.h"



void init_homography(homography_ctx *ctx, int max_points)
{
    ctx->max_points = max_points;
    ctx->base_points = gsl_matrix_alloc(max_points,2);
    ctx->world_points = gsl_matrix_alloc(max_points,3);
    ctx->homography_matrix = gsl_matrix_alloc(3,4);
    ctx->num_points = 0;
}

void free_homography(homography_ctx *ctx)
{
    gsl_matrix_free(ctx->base_points);
    gsl_matrix_free(ctx->world_points);
    gsl_matrix_free(ctx->homography_matrix);
}

int homography_add_points(homography_ctx *ctx, cone_laser_pos *laser_pos, cone_camera_pos *camera_pos)
{
    if(ctx->num_points + 1 < ctx->max_points) {

        gsl_matrix_set(ctx->world_points, ctx->num_points, 0, laser_pos->x1);
        gsl_matrix_set(ctx->world_points, ctx->num_points, 1, laser_pos->y1);
        gsl_matrix_set(ctx->world_points, ctx->num_points, 2, laser_pos->z);

        gsl_matrix_set(ctx->base_points, ctx->num_points, 0, camera_pos->x1);
        gsl_matrix_set(ctx->base_points, ctx->num_points, 1, camera_pos->y1);

        ctx->num_points++;

        gsl_matrix_set(ctx->world_points, ctx->num_points, 0, laser_pos->x2);
        gsl_matrix_set(ctx->world_points, ctx->num_points, 1, laser_pos->y2);
        gsl_matrix_set(ctx->world_points, ctx->num_points, 2, laser_pos->z);

        gsl_matrix_set(ctx->base_points, ctx->num_points, 0, camera_pos->x2);
        gsl_matrix_set(ctx->base_points, ctx->num_points, 1, camera_pos->y2);

        ctx->num_points++;
        
        return 1;
    }
   
    return 0;
}


void compute_homography(homography_ctx *ctx)
{
    int i;
    gsl_matrix * A = gsl_matrix_calloc(2 * ctx->num_points, 12);

    // Compute Matrix A (See trucco and Verri, chapter 6, pag133)
    for(i = 0; i < ctx->num_points; i++) {
        //printf("i=%d\ctx->num_points",i);
        gsl_matrix_set(A, 2 * i, 0, gsl_matrix_get(ctx->world_points, i, 0)); // X(i)
        gsl_matrix_set(A, 2 * i, 1, gsl_matrix_get(ctx->world_points, i, 1)); // Y(i)
        gsl_matrix_set(A, 2 * i, 2, gsl_matrix_get(ctx->world_points, i, 2)); // Z(i)
        gsl_matrix_set(A, 2 * i, 3, 1);

        gsl_matrix_set(A, 2 * i, 4, 0);
        gsl_matrix_set(A, 2 * i, 5, 0);
        gsl_matrix_set(A, 2 * i, 6, 0);
        gsl_matrix_set(A, 2 * i, 7, 0);

        gsl_matrix_set(A, 2 * i, 8, -gsl_matrix_get(ctx->base_points, i, 0) * gsl_matrix_get(ctx->world_points, i, 0)); // -u(i)*X(i)
        gsl_matrix_set(A, 2 * i, 9, -gsl_matrix_get(ctx->base_points, i, 0) * gsl_matrix_get(ctx->world_points, i, 1)); // -u(i)*Y(i)
        gsl_matrix_set(A, 2 * i, 10, -gsl_matrix_get(ctx->base_points, i, 0) * gsl_matrix_get(ctx->world_points, i, 2)); // -u(i)*Z(i)
        gsl_matrix_set(A, 2 * i, 11, -gsl_matrix_get(ctx->base_points, i, 0)); // -u(i)


        gsl_matrix_set(A, 2 * i + 1, 0, 0);
        gsl_matrix_set(A, 2 * i + 1, 1, 0);
        gsl_matrix_set(A, 2 * i + 1, 2, 0);
        gsl_matrix_set(A, 2 * i + 1, 3, 0);

        gsl_matrix_set(A, 2 * i + 1, 4, gsl_matrix_get(ctx->world_points, i, 0)); // X(i)
        gsl_matrix_set(A, 2 * i + 1, 5, gsl_matrix_get(ctx->world_points, i, 1)); // Y(i)
        gsl_matrix_set(A, 2 * i + 1, 6, gsl_matrix_get(ctx->world_points, i, 2)); // Z(i)
        gsl_matrix_set(A, 2 * i + 1, 7, 1);

        gsl_matrix_set(A, 2 * i + 1, 8, -gsl_matrix_get(ctx->base_points, i, 1) * gsl_matrix_get(ctx->world_points, i, 0)); // -v(i)*X(i)
        gsl_matrix_set(A, 2 * i + 1, 9, -gsl_matrix_get(ctx->base_points, i, 1) * gsl_matrix_get(ctx->world_points, i, 1)); // -v(i)*X(i)
        gsl_matrix_set(A, 2 * i + 1, 10, -gsl_matrix_get(ctx->base_points, i, 1) * gsl_matrix_get(ctx->world_points, i, 2)); // -v(i)*X(i)
        gsl_matrix_set(A, 2 * i + 1, 11, -gsl_matrix_get(ctx->base_points, i, 1)); // -v(i)

    }

    gsl_matrix * V = gsl_matrix_calloc(12, 12);
    gsl_vector * S = gsl_vector_calloc(12);
    gsl_vector * work = gsl_vector_calloc(12);

    // SVD of matrix A
    gsl_linalg_SV_decomp(A, V, S, work);

    // Find the rank of A
    int N = 11;
    while((gsl_vector_get(S, N) < 0.00001) && (N > 0))
        N--;

    //if(N == 11)
    //    printf("Rank OK!\ctx->num_points");

    gsl_vector * m = gsl_vector_calloc(12);
    // Get the column of V which corresponds to the small singular value different than zero
    gsl_matrix_get_col(m, V, N);

    // Compute matrix ctx->homography_matrix (homography)
    gsl_matrix_set(ctx->homography_matrix, 0, 0, gsl_vector_get(m, 0));
    gsl_matrix_set(ctx->homography_matrix, 0, 1, gsl_vector_get(m, 1));
    gsl_matrix_set(ctx->homography_matrix, 0, 2, gsl_vector_get(m, 2));
    gsl_matrix_set(ctx->homography_matrix, 0, 3, gsl_vector_get(m, 3));

    gsl_matrix_set(ctx->homography_matrix, 1, 0, gsl_vector_get(m, 4));
    gsl_matrix_set(ctx->homography_matrix, 1, 1, gsl_vector_get(m, 5));
    gsl_matrix_set(ctx->homography_matrix, 1, 2, gsl_vector_get(m, 6));
    gsl_matrix_set(ctx->homography_matrix, 1, 3, gsl_vector_get(m, 7));

    gsl_matrix_set(ctx->homography_matrix, 2, 0, gsl_vector_get(m, 8));
    gsl_matrix_set(ctx->homography_matrix, 2, 1, gsl_vector_get(m, 9));
    gsl_matrix_set(ctx->homography_matrix, 2, 2, gsl_vector_get(m, 10));
    gsl_matrix_set(ctx->homography_matrix, 2, 3, gsl_vector_get(m, 11));

    //for(i=1;i<12;i++)
    //	printf("m[%d]=%f\ctx->num_points",i,gsl_vector_get(m,i));

}


// Given the image coordinate of a point (u,v), the homography matrix ctx->homography_matrix, and the component Z (zpos) of the world coordinate of a point,
// this function computes the coordinate X (xpos) and Y (ypos) of the point

void image2world(homography_ctx *ctx, int u, int v, double zpos, double *xpos, double *ypos)
{
	double a = gsl_matrix_get(ctx->homography_matrix,0,0);
	double b = gsl_matrix_get(ctx->homography_matrix,0,1);
	double c = gsl_matrix_get(ctx->homography_matrix,0,2)*zpos + gsl_matrix_get(ctx->homography_matrix,0,3);
	double d = gsl_matrix_get(ctx->homography_matrix,1,0);
	double e = gsl_matrix_get(ctx->homography_matrix,1,1);
	double f = gsl_matrix_get(ctx->homography_matrix,1,2)*zpos + gsl_matrix_get(ctx->homography_matrix,1,3);
	double g = gsl_matrix_get(ctx->homography_matrix,2,0);
	double h = gsl_matrix_get(ctx->homography_matrix,2,1);
	double i = gsl_matrix_get(ctx->homography_matrix,2,2)*zpos + gsl_matrix_get(ctx->homography_matrix,1,3);

	*ypos = (c*d - v*c*g - u*d*i + u*f*g - a*f + v*a*i) / (a*e - u*e*g - v*a*h - b*d + u*d*h + v*b*g);
	*xpos = ((b - u*h) * (*ypos) + c - u*i) / (u*g - a);
}


// Given the world coordinate of a point (xpos,ypos,zpos) and the homography matrix ctx->homography_matrix, this function computes the
// image coordinates of this point (projection of this point on the image plane)
void world2image(homography_ctx *ctx, double xpos, double ypos, double zpos, int *u, int *v)
{
	//printf("%f %f %f\n",xpos,ypos,zpos);
	double a = gsl_matrix_get(ctx->homography_matrix,0,0)*xpos;
	double b = gsl_matrix_get(ctx->homography_matrix,0,1)*ypos;
	double c = gsl_matrix_get(ctx->homography_matrix,0,2)*zpos + gsl_matrix_get(ctx->homography_matrix,0,3);
	double d = gsl_matrix_get(ctx->homography_matrix,1,0)*xpos;
	double e = gsl_matrix_get(ctx->homography_matrix,1,1)*ypos;
	double f = gsl_matrix_get(ctx->homography_matrix,1,2)*zpos + gsl_matrix_get(ctx->homography_matrix,1,3);
	double g = gsl_matrix_get(ctx->homography_matrix,2,0)*xpos;
	double h = gsl_matrix_get(ctx->homography_matrix,2,1)*ypos;
	double i = gsl_matrix_get(ctx->homography_matrix,2,2)*zpos + gsl_matrix_get(ctx->homography_matrix,2,3);

	*u = (int)round((a+b+c)/(g+h+i));
	*v = (int)round((d+e+f)/(g+h+i));
}