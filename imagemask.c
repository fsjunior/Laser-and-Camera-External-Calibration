#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "imagemask.h"

void init_image_mask(image_mask_ctx *ctx, homography_ctx *homography, obstacle_camera_detector_ctx *camera, playerc_laser_t *laser, double lazer_z_index)
{
    ctx->image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    ctx->homography = homography;
    ctx->laser = laser;
    ctx->lazer_z_index = lazer_z_index;
    ctx->camera = camera;
}

void compute_mask(image_mask_ctx *ctx)
{
    int i;
    double x1, y1, x2, y2;
    int u1, v1, u2, v2;

    //cvRectangle(ctx->image, cvPoint(0, 0), cvPoint(ctx->image->width, ctx->image->height), UNCERTAIN_REGION, 1, 8, 0);
    cvSet(ctx->image, BORDER_REGION, NULL);

    /*
        for(i = 0; i < ctx->laser->scan_count; i++) {
            if(!(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE && ctx->laser->ranges[i + 1] < MAX_LASER_READ_SIZE)) {
                calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
                world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

                calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i + 1], &x2, &y2);
                world2image(ctx->homography, x2, y2, 0, &u2, &v2);


                //DEFINE REGIÃO DE "BORDA"
                if(u1 >= 0 && v2 >= 0 && u2 >= 0)
                    cvRectangle(ctx->image, cvPoint(u1, v1), cvPoint(u2, 0), BORDER_REGION, CV_FILLED, 8, 0);
            }
        }
     */

    for(i = 0; i < ctx->laser->scan_count; i++) {
        //if(!(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE)) {
        calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
        world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

        //calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i + 1], &x2, &y2);
        //world2image(ctx->homography, x2, y2, 0, &u2, &v2);


        /*DEFINE REGIÃO DE BORDA */
        //if(u1 >= 0 && v2 >= 0 && u2 >= 0)
        cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_BORDER_WIDTH, v1), cvPoint(u1 + HALF_LINE_BORDER_WIDTH, ctx->image->height), NAVIGABLE_REGION, CV_FILLED, 8, 0);

        // }
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
            cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_OBSTACLE_WIDTH, 0), cvPoint(u1 + HALF_LINE_OBSTACLE_WIDTH, v1), BORDER_REGION, CV_FILLED, 8, 0);
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

void save_mask_as_arff(image_mask_ctx *ctx, char *filename)
{
    FILE *file;
    int i, j;
    CvScalar colour, maskcolour;

    file = fopen(filename, "wt");

    /* Print Header */

    fprintf(file, "@RELATION carimage\n\n");
    fprintf(file, "@ATTRIBUTE image_red NUMERIC\n");
    fprintf(file, "@ATTRIBUTE image_green NUMERIC\n");
    fprintf(file, "@ATTRIBUTE image_blue NUMERIC\n");
    fprintf(file, "@ATTRIBUTE class  {NON_NAVIGABLE,NAVIGABLE,BORDER}\n");
    fprintf(file, "\n@DATA\n");
    colour = NON_NAVIGABLE_REGION;
    fprintf(stdout, "%d,%d,%d,\n", (unsigned int)colour.val[0], (unsigned int)colour.val[1], (unsigned int)colour.val[2]);

    for(i = 0; i < ctx->image->width; i++) {
        for(j = 0; j < ctx->image->height; j++) {
            colour = cvGet2D(ctx->camera->image, j, i);
            maskcolour = cvGet2D(ctx->image, j, i);
            
            fprintf(file, "%d,%d,%d,", (unsigned int)colour.val[0], (unsigned int)colour.val[1], (unsigned int)colour.val[2]);

            if((unsigned int)maskcolour.val[2] == 255) {
                fprintf(file, "NON_NAVIGABLE\n");
            } else if((unsigned int)maskcolour.val[1] == 255) {
                fprintf(file, "NAVIGABLE\n");
            } else if((unsigned int)maskcolour.val[0] == 255) {
                fprintf(file, "BORDER\n");
            }



        }
    }


    fclose(file);
}
