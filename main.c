#include <stdio.h>
#include <libplayerc/playerc.h>
#include <string.h>
#include "laser.h"
//#include <gsl/gsl_matrix.h>


// centimetros

int main(int argc, char *argv[])
{
    playerc_client_t *client;
    playerc_laser_t *laser;
    cone_detector_ctx cone_detector;

    // Create a client and connect it to the server.
    client = playerc_client_create(NULL, "localhost", 6665);
    if(0 != playerc_client_connect(client))
        return -1;

    laser = playerc_laser_create(client, 0);
    if(playerc_laser_subscribe(laser, PLAYER_OPEN_MODE))
        return -1;


    init_cone_detector_ctx(&cone_detector, laser);


    do {
        if(!playerc_client_read(client))
            return -1;

        calc_cone_pos(&cone_detector);

        printf("x1:%f y1:%f x2:%f y2:%f z:%f\n", cone_detector.pos.x1, cone_detector.pos.y1, cone_detector.pos.x2, cone_detector.pos.y2, cone_detector.pos.z);
        fflush(stdout);
    } while(1);


    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 1;
}
