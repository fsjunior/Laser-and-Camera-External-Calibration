/*
 * File:   laser.c
 * Author: Francisco de Souza Júnior
 *
 * Created on March 29, 2011, 2:43 PM
 */


#include "laser.h"


//declarations
int get_minor_scan_index(playerc_laser_t *laser);
void get_cone_borders(playerc_laser_t *laser, int middle, int *left, int *right);
double get_laser_range_median(playerc_laser_t *laser, int max_index);
inline double calc_z(double dist);


//definitions

int get_minor_scan_index(playerc_laser_t *laser)
{
    int i;
    double minor_scan_data = 8.0;
    int minor_scan_index = -1;

    for(i = 0; i < laser->scan_count; i++) {
        if(laser->ranges[i] - minor_scan_data < 0.0001) {
            minor_scan_data = laser->ranges[i];
            minor_scan_index = i;
        }
    }

    return minor_scan_index;
}

void get_cone_borders(playerc_laser_t *laser, int middle, int *left, int *right)
{
    int i;
    i = middle + 1;
    while(i < laser->scan_count && laser->ranges[i] - laser->ranges[i - 1] < BORDER_THRESHOLD)
        i++;

    *left = i - 1;
    i = middle - 1;

    while(i >= 0 && laser->ranges[i] - laser->ranges[i + 1] < BORDER_THRESHOLD)
        i--;
    *right = i + 1;
}

inline double calc_z(double dist)
{
    //return dist * -3.3592264 + 0.9500;
    //return dist * -3.4605385 + 0.93684615;
    return dist * -3.4172781 + 0.9223438;
}

int compare(const void *a, const void *b)
{
    return( *(double*) a - *(double*) b);
}

double get_laser_range_median(playerc_laser_t *laser, int max_index)
{
    int i;
    double median_vector[MEDIAN_SIZE];

    for(i = 0; i < MEDIAN_SIZE; i++)
        median_vector[i] = laser->ranges[max_index - i];
    qsort(median_vector, MEDIAN_SIZE, sizeof(double), compare);
    return median_vector[MEDIAN_SIZE / 2];
}

void init_cone_laser_detector_ctx(cone_laser_detector_ctx *cone_detector, playerc_laser_t *laser)
{
    cone_detector->laser = laser;
    init_particle(cone_detector->leftranges, NUMBER_OF_RANGE_PARTICLES);
    init_particle(cone_detector->rightranges, NUMBER_OF_RANGE_PARTICLES);
}


void calc_cone_pos(cone_laser_detector_ctx *cone_detector)
{
    int minor_scan_index;
    int left, right;
    double leftrange, rightrange;


    minor_scan_index = get_minor_scan_index(cone_detector->laser);
    get_cone_borders(cone_detector->laser, minor_scan_index, &left, &right);

    leftrange = get_laser_range_median(cone_detector->laser, left);
    rightrange = get_laser_range_median(cone_detector->laser, right + MEDIAN_SIZE);

    add_particle(cone_detector->leftranges, leftrange, NUMBER_OF_RANGE_PARTICLES);
    add_particle(cone_detector->rightranges, rightrange, NUMBER_OF_RANGE_PARTICLES);

    leftrange = max_particle(cone_detector->leftranges, NUMBER_OF_RANGE_PARTICLES);
    rightrange = max_particle(cone_detector->rightranges, NUMBER_OF_RANGE_PARTICLES);

    calc_x_y(cone_detector->laser->scan_count, left, leftrange, &(cone_detector->pos.x1), &(cone_detector->pos.y1));
    calc_x_y(cone_detector->laser->scan_count, right, rightrange, &(cone_detector->pos.x2), &(cone_detector->pos.y2));

    cone_detector->pos.dist = calc_dist(cone_detector->pos.x1, cone_detector->pos.x2, cone_detector->pos.y1, cone_detector->pos.y2);

    cone_detector->pos.z = calc_z(cone_detector->pos.dist);
}



