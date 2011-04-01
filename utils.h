#include <gsl/gsl_matrix.h>


#ifndef _UTILS_H_
#define _UTILS_H_


#ifndef PARTICLE_VALUE_DECIMALS
#define PARTICLE_VALUE_DECIMALS 2
#endif


typedef struct {
    double x1, y1;
    double x2, y2;
    double dist;
    double z;
} cone_laser_pos;

typedef struct {
    int x1, y1;
    int x2, y2;
} cone_camera_pos;

typedef struct {
    int v; //value (fixed point)
    int n; //p(value)
} double_particle;

inline double degreetoradian(double degree);

void add_particle(double_particle *p, double value, int s);
double max_particle(double_particle *p, int s);
void init_particle(double_particle *p, int s);

void calc_x_y(int scan_count, int index, double range, double *x, double *y);
double calc_dist(double x1, double x2, double y1, double y2);




#endif /* _UTILS_H_ */