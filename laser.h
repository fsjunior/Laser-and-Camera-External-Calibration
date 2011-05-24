
#include <libplayerc/playerc.h>
#include "utils.h"

#ifndef _LASER_H_
#define _LASER_H_


#ifndef BORDER_THRESHOLD
#define BORDER_THRESHOLD 0.030
#endif

#ifndef MEDIAN_SIZE
#define MEDIAN_SIZE 3
#endif

#ifndef NUMBER_OF_RANGE_PARTICLES
#define NUMBER_OF_RANGE_PARTICLES 5
#endif

typedef struct {
//    double_particle leftranges[NUMBER_OF_RANGE_PARTICLES];
//    double_particle rightranges[NUMBER_OF_RANGE_PARTICLES];
    playerc_laser_t *laser;
    obstacle_laser_pos pos;
    int minor_scan_index;
} obstacle_laser_detector_ctx;


void init_obstacle_laser_detector_ctx(obstacle_laser_detector_ctx *obstacle_detector, playerc_laser_t *laser);
void calc_obstacle_laser_pos(obstacle_laser_detector_ctx *obstacle_detector);

#endif /* _LASER_H_ */