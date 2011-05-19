#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "homography.h"
#include "imagemask.h"

void init_image_mask(image_mask_ctx *ctx, homography_ctx *homography, playerc_laser_t *laser, double lazer_z_index)
{
    ctx->image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    ctx->homography = homography;
    ctx->laser = laser;
    ctx->lazer_z_index = lazer_z_index;
}

void compute_mask(image_mask_ctx *ctx)
{
    int i;
    double x1, y1, x2, y2;
    int u1, v1, u2, v2;

    //cvRectangle(ctx->image, cvPoint(0, 0), cvPoint(ctx->image->width, ctx->image->height), UNCERTAIN_REGION, 1, 8, 0);
    cvSet(ctx->image, NAVIGABLE_REGION, NULL);

    /*
        for(i = 0; i < ctx->laser->scan_count; i++) {
            if(!(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE && ctx->laser->ranges[i + 1] < MAX_LASER_READ_SIZE)) {
                calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
                world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

                calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i + 1], &x2, &y2);
                world2image(ctx->homography, x2, y2, 0, &u2, &v2);


                /*DEFINE REGIÃO DE "BORDA"
                if(u1 >= 0 && v2 >= 0 && u2 >= 0)
                    cvRectangle(ctx->image, cvPoint(u1, v1), cvPoint(u2, 0), BORDER_REGION, CV_FILLED, 8, 0);
            }
        }
     */

    for(i = 0; i < ctx->laser->scan_count; i++) {
        if(!(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE)) {
            calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
            world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

            //calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i + 1], &x2, &y2);
            //world2image(ctx->homography, x2, y2, 0, &u2, &v2);


            /*DEFINE REGIÃO DE BORDA */
            //if(u1 >= 0 && v2 >= 0 && u2 >= 0)
            cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_BORDER_WIDTH, v1), cvPoint(u1 + HALF_LINE_BORDER_WIDTH, 0), BORDER_REGION, CV_FILLED, 8, 0);

        }
    }

    for(i = 0; i < ctx->laser->scan_count; i++) {
        if(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE) {
            calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
            world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

            calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i], &x2, &y2);
            world2image(ctx->homography, x2, y2, 0, &u2, &v2);

            /*DEFINE REGIÃO NÃO-NAVEGÁVEL E DE BORDA*/
            /*define região não-navegável a partir do obstáculo até o chão (z = 0)*/
            //cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_WIDTH, v1), cvPoint(u1 + HALF_LINE_WIDTH, 0), BORDER_REGION, CV_FILLED, 8, 0);
            cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_OBSTACLE_WIDTH, 0), cvPoint(u1 + HALF_LINE_OBSTACLE_WIDTH, ctx->image->height), BORDER_REGION, CV_FILLED, 8, 0);
            cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_OBSTACLE_WIDTH, v1), cvPoint(u1 + HALF_LINE_OBSTACLE_WIDTH, v2), NON_NAVIGABLE_REGION, CV_FILLED, 8, 0);

            /*define região não-navegável do ponto da leitura do laser até o topo da imagem*/
            //cvRectangle(ctx->image, cvPoint(u1, v1), cvPoint(u2, 0), NON_NAVIGABLE_REGION, CV_FILLED, 8, 0);
        }
    }



}

void free_image_mask(image_mask_ctx *ctx)
{
    cvReleaseImage(&(ctx->image));
}

