#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#define TEST

bool float_equal(float a, float b) {
    return fabs(a - b) < 0.0001;
}

void test_update_vehicles(void) {

    // We create a custom lane with vehicles
    lane* l = (lane*)malloc(sizeof(lane));
    l->type = ROAD;
    l->y = 0;
    l->speed = VEHICLE_SPEED_MIN + (float)(rand()%2) / 10;
    l->coins = NULL;
    l->prev = NULL;
    l->next = NULL;
    l->obst_size = 3;


    // We create a unique obstacle we are going to track
    l->obstacles = (obstacle*)malloc(sizeof(obstacle));
    float x = rand()%(LANE_WIDTH - 4);
    l->obstacles->x = x;
    l->obstacles->size = 3;
    l->obstacles->next = NULL;
    l->obstacles->prev = NULL;


    // We create a unique obstacle we are going to track
    update_vehicles(l);


    // Check if the vehicle we created still exists and is in the right position
    obstacle* current = l->obstacles;
    bool did_move_correctly = false;
    while (current != NULL) {
        if(float_equal(current->x - l->speed, x)) {
            did_move_correctly = true;
        }
        current = current->next;
    }
    assert(did_move_correctly);


    free_obstacles(l->obstacles);
    free(l);
    printf("test_update_vehicles passed\n");
}

void test_trees(void) {

    // We create a custom lane with waterlilies before a lane of trees
    lane* waterlilies = (lane*)malloc(sizeof(lane));
    waterlilies->type = WATER;
    waterlilies->y = 0;
    waterlilies->speed = 0;
    waterlilies->coins = NULL;
    waterlilies->prev = NULL;
    waterlilies->obst_size = 1;
    waterlilies->obstacles = generate_waterlilies(waterlilies);
    

    // We create a custom lane with trees
    lane* trees = (lane*)malloc(sizeof(lane));
    trees->type = GRASS;
    trees->y = 1;
    trees->speed = 0;
    trees->coins = NULL;
    trees->prev = waterlilies;
    trees->obst_size = 1;
    trees->obstacles = generate_trees(trees);
    waterlilies->next = trees;
    trees->next = NULL;


    bool* prev = create_obstacles_array(waterlilies->obstacles);
    bool* current = create_obstacles_array(trees->obstacles);
    array_not(current);
    bool* res = array_and(prev, current);
    assert(array_exist(res));
    free(prev);
    free(current);
    free(res);
    free_lanes(waterlilies);
    printf("test_trees passed\n");

}


void test_update_drowning_slots(void) {

    // We create a custom lane with drowning slots
    lane* l = (lane*)malloc(sizeof(lane));
    l->type = WATER;
    l->y = 0;
    l->speed = LOG_SPEED * (rand()%2 ? 1 : -1);
    l->obst_size = 3;
    l->prev = NULL;
    l->next = NULL;


    // We create a unique obstacle we are going to track
    l->obstacles = (obstacle*)malloc(sizeof(obstacle));
    float x = rand()%(LANE_WIDTH - 5) + 2;
    l->obstacles->x = x;
    l->obstacles->size = 3;
    l->obstacles->next = NULL;
    l->obstacles->prev = NULL;


    // We create a unique coin we are going to track
    l->coins = (float_list*)malloc(sizeof(float_list));
    l->coins->val = x + 1;
    l->coins->next = NULL;


    // We update the drowning slots of the lane
    update_drowning_slots(l);


    // Check if the drowning slot we created still exists and is in the right position
    obstacle* current = l->obstacles;
    bool did_move_correctly = false;
    while (current != NULL) {
        if(float_equal(current->x - l->speed, x)) {
            did_move_correctly = true;
        }
        current = current->next;
    }
    assert(did_move_correctly);

    // Check if the coin we created still exists and is in the right position
    float_list* current_coin = l->coins;
    did_move_correctly = false;
    while (current_coin != NULL) {
        if(float_equal(current_coin->val - l->speed, x + 1)) {
            did_move_correctly = true;
        }
        current_coin = current_coin->next;
    }
    assert(did_move_correctly);


    free_obstacles(l->obstacles);
    free_coins(l->coins);
    free(l);
    printf("test_drowning_slots passed\n");

}

void test_trains(void) {
    
    // We create a custom lane with trains
    lane* l = (lane*)malloc(sizeof(lane));
    l->type = TRACK;
    l->y = 0;
    l->speed = TRAIN_SPEED * (rand()%2 ? 1 : -1);
    l->coins = NULL;
    l->prev = NULL;
    l->next = NULL;
    l->obst_size = TRAIN_LENGTH;


    // We create a unique obstacle we are going to track
    l->obstacles = (obstacle*)malloc(sizeof(obstacle));
    float x = rand()%(LANE_WIDTH - 5) + 2;
    l->obstacles->x = x;
    l->obstacles->size = TRAIN_LENGTH;
    l->obstacles->next = NULL;
    l->obstacles->prev = NULL;


    // We update the trains of the lane
    update_trains(l);


    // Check if the train we created still exists and is in the right position
    obstacle* current = l->obstacles;
    bool did_move_correctly = false;
    while (current != NULL) {
        if(float_equal(current->x - l->speed, x)) {
            did_move_correctly = true;
        }
        current = current->next;
    }
    assert(did_move_correctly);


    free_obstacles(l->obstacles);
    free(l);
    printf("test_trains passed\n");
}


int main(void) {
    srand(time(NULL));
    test_update_vehicles();
    test_trees();
    test_update_drowning_slots();
    test_trains();
    return 0;
}
