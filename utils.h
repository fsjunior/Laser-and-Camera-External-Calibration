#include <gsl/gsl_matrix.h>


#ifndef _UTILS_H_
#define _UTILS_H_


#ifndef PARTICLE_VALUE_DECIMALS
#define PARTICLE_VALUE_DECIMALS 2
#endif


typedef struct {
    double x;
    double y;
} obstacle_laser_pos;

typedef struct {
    int x[2];
    int y[2];
} obstacle_camera_pos;

typedef struct {
    int v; //value (fixed point)
    int n; //p(value)
} double_particle;

inline double degreetoradian(double degree);

void add_particle(double_particle *p, double value, int s);
double max_particle(double_particle *p, int s);
void init_particle(double_particle *p, int s);

void calc_x_y(int scan_count, int index, double range, double *x, double *y);
double calc_dist(obstacle_laser_pos *pos);




#endif /* _UTILS_H_ */