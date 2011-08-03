#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "imagemask.h"
#include "math.h"

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


    for(i = 0; i < ctx->laser->scan_count; i++) {
        //if(!(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE)) {
        calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
        world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

        //calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i + 1], &x2, &y2);
        //world2image(ctx->homography, x2, y2, 0, &u2, &v2);


        /*DEFINE REGIÃO NAVEGAVEL */
        //if(u1 >= 0 && v2 >= 0 && u2 >= 0)
        cvRectangle(ctx->image, cvPoint(u1 - HALF_LINE_NAVIGABLE_WIDTH, v1), cvPoint(u1 + HALF_LINE_NAVIGABLE_WIDTH, ctx->image->height), NAVIGABLE_REGION, CV_FILLED, 8, 0);

        // }
    }

    for(i = 0; i < ctx->laser->scan_count; i++) {
        if(ctx->laser->ranges[i] < MAX_LASER_READ_SIZE) {
            calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x1, &y1);
            world2image(ctx->homography, x1, y1, ctx->lazer_z_index, &u1, &v1);

            calc_x_y(ctx->laser->scan_count, i + 1, ctx->laser->ranges[i], &x2, &y2);
            world2image(ctx->homography, x2, y2, 0, &u2, &v2);

            /*DEFINE REGIÃO NÃO-NAVEGÁVEL E DE BORDA*/
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

float calc_mean(float *values, int n)
{
    int i;
    float result = 0;
    for(i = 0; i < n; i++)
        result += values[i];
    return result / (float) n;
}

float calc_variance(float *values, float mean, int n)
{
    int i;
    float result = 0;
    for(i = 0; i < n; i++)
        result += (values[i] - mean) * (values[i] - mean);
    return result / (float) (n - 1);
}

float calc_skweness(float *values, float mean, float variance, int n)
{
    int i;
    float result = 0;
    float std_dev_3 = sqrt(variance);
    std_dev_3 = std_dev_3 * std_dev_3 * std_dev_3;

    for(i = 0; i < n; i++)
        result += (values[i] - mean) * (values[i] - mean) * (values[i] - mean);
    return result / ((float) (n - 1) * std_dev_3);
}

/*
void save_mask_as_arff(image_mask_ctx *ctx, char *filename)
{
    FILE *file;
    int i, j, ib, jb;
    float *red_pointer, *red = malloc(sizeof(float) * (BLOCK_SIZE * BLOCK_SIZE));
    float *green_pointer, *green = malloc(sizeof(float) * (BLOCK_SIZE * BLOCK_SIZE));
    float *blue_pointer, *blue = malloc(sizeof(float) * (BLOCK_SIZE * BLOCK_SIZE));

    float red_mean, green_mean, blue_mean, red_variance, green_variance,
            blue_variance, red_skweness, green_skweness, blue_skweness;

    CvScalar colour, maskcolour;

    file = fopen(filename, "wt");

    // Print Header

    fprintf(file, "@RELATION carimage\n\n");
    fprintf(file, "@ATTRIBUTE image_red_mean REAL\n");
    fprintf(file, "@ATTRIBUTE image_green_mean REAL\n");
    fprintf(file, "@ATTRIBUTE image_blue_mean REAL\n");
    fprintf(file, "@ATTRIBUTE image_red_variance REAL\n");
    fprintf(file, "@ATTRIBUTE image_green_variance REAL\n");
    fprintf(file, "@ATTRIBUTE image_blue_variance REAL\n");
    fprintf(file, "@ATTRIBUTE image_red_skweness REAL\n");
    fprintf(file, "@ATTRIBUTE image_green_skweness REAL\n");
    fprintf(file, "@ATTRIBUTE image_blue_skweness REAL\n");
*/
    //fprintf(file, "@ATTRIBUTE image_red_kurtosis REAL\n");
    //fprintf(file, "@ATTRIBUTE image_green_kurtosis REAL\n");
    //fprintf(file, "@ATTRIBUTE image_blue_kurtosis REAL\n");
    /*
        fprintf(file, "@ATTRIBUTE image_red_norm_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_green_norm_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_blue_norm_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_red_norm_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_green_norm_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_blue_norm_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_red_norm_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_green_norm_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_blue_norm_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_red_norm_kurtosis REAL\n");
        fprintf(file, "@ATTRIBUTE image_green_norm_kurtosis REAL\n");
        fprintf(file, "@ATTRIBUTE image_blue_norm_kurtosis REAL\n");
        fprintf(file, "@ATTRIBUTE image_y_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_u_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_v_mean REAL\n");
        fprintf(file, "@ATTRIBUTE image_y_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_u_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_v_variance REAL\n");
        fprintf(file, "@ATTRIBUTE image_y_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_u_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_v_skweness REAL\n");
        fprintf(file, "@ATTRIBUTE image_y_kurtosis REAL\n");
        fprintf(file, "@ATTRIBUTE image_u_kurtosis REAL\n");
        fprintf(file, "@ATTRIBUTE image_v_kurtosis REAL\n");
     */




/*
    fprintf(file, "@ATTRIBUTE class  {NON_NAVIGABLE,NAVIGABLE,BORDER}\n");
    fprintf(file, "\n@DATA\n");
    colour = NON_NAVIGABLE_REGION;
    fprintf(stdout, "%d,%d,%d,\n", (unsigned int) colour.val[0], (unsigned int) colour.val[1], (unsigned int) colour.val[2]);

    for(i = 0; i < ctx->image->width; i += BLOCK_SIZE) {
        for(j = 0; j < ctx->image->height; j += BLOCK_SIZE) {

            red_pointer = red;
            green_pointer = red;
            blue_pointer = red;

            for(ib = 0; ib < BLOCK_SIZE; ib++) {
                for(jb = 0; jb < BLOCK_SIZE; jb++) {
                    colour = cvGet2D(ctx->camera->image, j + jb, i + ib);
                    *red_pointer = colour.val[2];
                    red_pointer++;
                    *green_pointer = colour.val[1];
                    green_pointer++;
                    *blue_pointer = colour.val[0];
                    blue_pointer++;

                    maskcolour = cvGet2D(ctx->image, j + jb, i + ib);


                }
            }

            red_mean = calc_mean(red, BLOCK_SIZE * BLOCK_SIZE);
            green_mean = calc_mean(green, BLOCK_SIZE * BLOCK_SIZE);
            blue_mean = calc_mean(blue, BLOCK_SIZE * BLOCK_SIZE);

            red_variance = calc_variance(red, red_mean, BLOCK_SIZE * BLOCK_SIZE);
            green_variance = calc_variance(green, green_mean, BLOCK_SIZE * BLOCK_SIZE);
            blue_variance = calc_variance(blue, blue_mean, BLOCK_SIZE * BLOCK_SIZE);

            red_skweness = calc_skweness(red, red_mean, red_variance, BLOCK_SIZE * BLOCK_SIZE);
            green_skweness = calc_skweness(green, green_mean, green_variance, BLOCK_SIZE * BLOCK_SIZE);
            blue_skweness = calc_skweness(blue, blue_mean, blue_variance, BLOCK_SIZE * BLOCK_SIZE);

            printf("%.3f,%.3f,%.3f,", red_mean, green_mean, blue_mean);
            printf("%.3f,%.3f,%.3f,", red_variance, green_variance, blue_variance);
            printf("%.3f,%.3f,%.3f,", red_skweness, green_skweness, blue_skweness);






            maskcolour = cvGet2D(ctx->image, j, i);

            if((unsigned int) maskcolour.val[2] == 255) {
                fprintf(file, "NON_NAVIGABLE\n");
            } else if((unsigned int) maskcolour.val[1] == 255) {
                fprintf(file, "NAVIGABLE\n");
            } else if((unsigned int) maskcolour.val[0] == 255) {
                fprintf(file, "BORDER\n");
            }



        }
    }


    fclose(file);
    free(red);
    free(green);
    free(blue);
}
*/
