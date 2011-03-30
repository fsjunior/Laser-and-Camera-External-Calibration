/*
 * File:   utils.c
 * Author: Francisco de Souza Júnior
 *
 * Created on March 29, 2011, 2:43 PM
 */

#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <string.h> 


void calc_x_y(int scan_count, int index, double range, double *x, double *y)
{
    double angle = degreetoradian((180.0 / (double) scan_count) * (double) index);

    *x = cos(angle) * range;
    *y = sin(angle) * range;
}

double calc_dist(double x1, double x2, double y1, double y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

inline double degreetoradian(double degree)
{
    return degree * (M_PI / 180.0);
}

void add_particle(double_particle *p, double value, int s)
{
    int i;
    int less = 0;
    int v = floor(value * pow(10, PARTICLE_VALUE_DECIMALS));

    for (i = 0; i < s; i++) {
        if (p[i].v == v) {
            if (p[i].n < 100)
                p[i].n += 10;
        } else if (p[i].n > 0)
            p[i].n -= 5;
        if (p[i].n < p[less].n) {
            less = i;
        }
    }

    p[less].v = v;
    p[less].n = 10;
}

double max_particle(double_particle *p, int s)
{
    int i, t = 0;
    for (i = 0; i < s; i++) {
        if (p[i].n > p[t].n)
            t = i;
    }
    return (double) p[t].v / pow(10, PARTICLE_VALUE_DECIMALS);
}

void init_particle(double_particle *p, int s)
{
    memset(p, 0, sizeof (double_particle) * s);
}

