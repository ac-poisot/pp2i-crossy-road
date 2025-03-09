#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#define TEST

int main(void) {
    srand(time(NULL));
    lane* l = generate_lane(NULL, ROAD);
    display_obstacles(l->obstacles);
    printf("\n");
    float first_x = l->obstacles->x;
    obstacle *last_obst = l->obstacles;
    while (last_obst->next != NULL) {
        last_obst = last_obst->next;
    }
    float last_x = last_obst->x;
    update_vehicles(l);
    display_obstacles(l->obstacles);
    printf("\n");
    assert(l->obstacles != NULL);
    assert(((int)l->obstacles->x == (int)first_x + (int)l->speed) || ((int) l->obstacles->x == (int)first_x + (int)l->speed - 6) || ((int) l->obstacles->x == (int)first_x + (int)l->speed - 12));
    last_obst = l->obstacles;
    while (last_obst->next != NULL) {
        last_obst = last_obst->next;
    }
    float new_last_x = last_obst->x;
    assert((last_x + l->speed == new_last_x) || (last_x + l->speed - 6 == new_last_x) || (last_x + l->speed - 12 == new_last_x) || (last_x >= LANE_WIDTH && (last_x + l->speed - 18 == new_last_x || last_x + l->speed - 24 == new_last_x)));
}
