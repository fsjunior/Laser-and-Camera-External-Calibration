#include "laser.h"


//declarations
int get_minor_scan_index(playerc_laser_t *laser);
void get_obstacle_borders(playerc_laser_t *laser, int middle, int *left, int *right);
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

void get_obstacle_borders(playerc_laser_t *laser, int middle, int *left, int *right)
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

void init_obstacle_laser_detector_ctx(obstacle_laser_detector_ctx *obstacle_detector, playerc_laser_t *laser)
{
    obstacle_detector->laser = laser;
    obstacle_detector->minor_scan_index = -1;
//    init_particle(obstacle_detector->leftranges, NUMBER_OF_RANGE_PARTICLES);
//    init_particle(obstacle_detector->rightranges, NUMBER_OF_RANGE_PARTICLES);
}

void calc_obstacle_laser_pos(obstacle_laser_detector_ctx *obstacle_detector)
{
//    int minor_scan_index;
/*
    int left, right;
    double leftrange, rightrange;
*/


    obstacle_detector->minor_scan_index = get_minor_scan_index(obstacle_detector->laser);
/*    get_obstacle_borders(obstacle_detector->laser, minor_scan_index, &left, &right);

    leftrange = get_laser_range_median(obstacle_detector->laser, left);
    rightrange = get_laser_range_median(obstacle_detector->laser, right + MEDIAN_SIZE);

    add_particle(obstacle_detector->leftranges, leftrange, NUMBER_OF_RANGE_PARTICLES);
    add_particle(obstacle_detector->rightranges, rightrange, NUMBER_OF_RANGE_PARTICLES);

    leftrange = max_particle(obstacle_detector->leftranges, NUMBER_OF_RANGE_PARTICLES);
    rightrange = max_particle(obstacle_detector->rightranges, NUMBER_OF_RANGE_PARTICLES);

    calc_x_y(obstacle_detector->laser->scan_count, left, leftrange, &(obstacle_detector->pos.x[0]), &(obstacle_detector->pos.y[0]));
    calc_x_y(obstacle_detector->laser->scan_count, right, rightrange, &(obstacle_detector->pos.x[1]), &(obstacle_detector->pos.y[1]));*/

    calc_x_y(obstacle_detector->laser->scan_count, obstacle_detector->minor_scan_index, obstacle_detector->laser->ranges[obstacle_detector->minor_scan_index], &(obstacle_detector->pos.x), &(obstacle_detector->pos.y));

}



