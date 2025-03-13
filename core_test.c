#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#define TEST

bool* generateObtaclePresenceArray(lane* l) {
    //bool obstacle_positions[LANE_WIDTH] = { false };
    bool *obstacle_positions = (bool*)malloc(LANE_WIDTH * sizeof(bool));
    for(int i = 0; i < LANE_WIDTH; i++) {
        obstacle_positions[i] = false;
    }
    obstacle *current_obst = l->obstacles;
    while (current_obst != NULL) {
        int start = (int)current_obst->x;
        int end = start + current_obst->size;
        for (int i = start; i < end && i < LANE_WIDTH; i++) {
            if(i >= 0 && i < LANE_WIDTH) {
                obstacle_positions[i] = true;
            }
        }
        current_obst = current_obst->next;
    }

    return obstacle_positions;
}

void test_vehicles(void) {
    lane* l = generate_lane(NULL, ROAD);
    displayLanes(l);
    bool* obstacle_positions = generateObtaclePresenceArray(l);
    float first_x = l->obstacles->x;
    obstacle *last_obst = l->obstacles;
    while (last_obst->next != NULL) {
        last_obst = last_obst->next;
    }
    float last_x = last_obst->x;
    update_vehicles(l);
    displayLanes(l);
    printf("\n");
    assert(l->obstacles != NULL);bool* new_obstacle_positions = generateObtaclePresenceArray(l);
    for(int i = 0; i < LANE_WIDTH; i++) {
        if(i+l->speed >= 0 && i+l->speed < LANE_WIDTH) {
            assert(obstacle_positions[i] == new_obstacle_positions[i+(int)l->speed]);
        }
    }
    free(obstacle_positions);
    free(new_obstacle_positions);
    assert(((int)l->obstacles->x == (int)first_x + (int)l->speed) || ((int) l->obstacles->x == (int)first_x + (int)l->speed - 6) || ((int) l->obstacles->x == (int)first_x + (int)l->speed - 12) || ((int)l->obstacles->x == (int)first_x + (int)l->speed +6) || ((int)l->obstacles->x == (int)first_x + (int)l->speed +12) || ((int) l->obstacles->x == (int)first_x + (int)l->speed + 18));
    last_obst = l->obstacles;
    while (last_obst->next != NULL) {
        last_obst = last_obst->next;
    }
    float new_last_x = last_obst->x;
    assert((last_x + l->speed == new_last_x) || (last_x + l->speed - 6 == new_last_x) || (last_x + l->speed - 12 == new_last_x) || (last_x + l->speed - 18 == new_last_x) || (last_x + l->speed + 6 == new_last_x) || (last_x + l->speed + 12 == new_last_x));
    free_lanes(l);
}


void test_drowning_slots(void) {
    lane* l = generate_lane(NULL, WATER);
    displayLanes(l);
    bool* obstacle_positions = generateObtaclePresenceArray(l);
    update_drowning_slots(l);
    displayLanes(l);
    printf("\n");
    assert(l->obstacles != NULL);
    bool* new_obstacle_positions = generateObtaclePresenceArray(l);
    for(int i = 0; i < LANE_WIDTH; i++) {
        if(i+l->speed >= 0 && i+l->speed < LANE_WIDTH) {
            assert(obstacle_positions[i] == new_obstacle_positions[i+(int)l->speed]);
        }
    }
    free(obstacle_positions);
    free(new_obstacle_positions);
    free_lanes(l);

}

void test_trains(void) {
    lane* l = generate_lane(NULL, TRACK);
    displayLanes(l);
    bool* obstacle_positions = generateObtaclePresenceArray(l);
    update_vehicles(l);
    displayLanes(l);
    printf("\n");
    assert(l->obstacles != NULL);
    bool* new_obstacle_positions = generateObtaclePresenceArray(l);
    for(int i = 0; i < LANE_WIDTH; i++) {
        if(i+l->speed >= 0 && i+l->speed < LANE_WIDTH) {
            assert(obstacle_positions[i] == new_obstacle_positions[i+(int)l->speed]);
        }
    }
    free(obstacle_positions);
    free(new_obstacle_positions);
    free_lanes(l);
}

int main(void) {
    srand(time(NULL));
    test_vehicles();
    test_drowning_slots();
    test_trains();
    return 0;
}
