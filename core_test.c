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

void test_trees_generation(void) {

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

void test_update_trains(void) {
    
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


void test_waterlilies_generation_1(void) {
    /*
    Tests the case where 2 lanes of waterlilies are separated by one lane of trees
    */

    // We create a custom lane with waterlilies
    lane* waterlilies = (lane*)malloc(sizeof(lane));
    waterlilies->type = WATER;
    waterlilies->y = 0;
    waterlilies->speed = 0;
    waterlilies->coins = NULL;
    waterlilies->prev = NULL;
    waterlilies->next = NULL;
    waterlilies->obst_size = 1;
    waterlilies->obstacles = generate_waterlilies(waterlilies);


    // We create a custom lane of trees
    lane* trees = generate_lane(waterlilies, GRASS);

    // We create a custom lane of waterlilies
    lane* waterlilies2 = (lane*)malloc(sizeof(lane));
    waterlilies2->type = WATER;
    waterlilies2->y = 2;
    waterlilies2->speed = 0;
    waterlilies2->coins = NULL;
    waterlilies2->prev = trees;
    waterlilies2->next = NULL;
    waterlilies2->obst_size = 1;
    waterlilies2->obstacles = generate_waterlilies(waterlilies2);
    trees->next = waterlilies2;

    bool* waterlilies_array = create_obstacles_array(waterlilies->obstacles);
    bool* waterlilies2_array = create_obstacles_array(waterlilies2->obstacles);
    bool* trees_array = create_obstacles_array(trees->obstacles);

    bool* reachable = (bool*)malloc(LANE_WIDTH * sizeof(bool));
    for(int i = 0; i < LANE_WIDTH; i++) {
        reachable[i] = false;
    }
    for(int i = 0; i < LANE_WIDTH; i++) {
        if(waterlilies_array[i] && !trees_array[i]) {
            reachable[i] = true;
            int j = i;
            while(j < LANE_WIDTH && !trees_array[j]) {
                reachable[j] = true;
                j++;
            }
            j = i;
            while(j >= 0 && !trees_array[j]) {
                reachable[j] = true;
                j--;
            }
        }
    }
    for(int i = 0; i < UNPLAYABLE_WIDTH; i++) {
        reachable[i] = false;
    }
    for(int i = LANE_WIDTH - UNPLAYABLE_WIDTH; i < LANE_WIDTH; i++) {
        reachable[i] = false;
    }

    bool* res = array_and(reachable, waterlilies2_array);
    assert(array_exist(res));
    free(res);
    free(waterlilies_array);
    free(reachable);
    free(waterlilies2_array);
    free(trees_array);
    free_lanes(waterlilies);
    printf("test_waterlilies_generation_1 passed\n");
}

void test_waterlilies_generation_2(void) {
    /*
    Tests the case where a lane of trees is followed by a lane of waterlilies
    */

    // We create a custom lane with trees
    lane* trees = generate_lane(NULL, GRASS);


    // We create a custom lane with waterlilies
    lane* waterlilies = (lane*)malloc(sizeof(lane));
    waterlilies->type = WATER;
    waterlilies->y = 1;
    waterlilies->speed = 0;
    waterlilies->coins = NULL;
    waterlilies->prev = trees;
    waterlilies->next = NULL;
    waterlilies->obst_size = 1;
    waterlilies->obstacles = generate_waterlilies(waterlilies);
    trees->next = waterlilies;

    bool* trees_array = create_obstacles_array(trees->obstacles);
    bool* waterlilies_array = create_obstacles_array(waterlilies->obstacles);
    array_not(trees_array);
    bool* res = array_and(trees_array, waterlilies_array);
    assert(array_exist(res));
    free(res);
    free(trees_array);
    free(waterlilies_array);
    free_lanes(trees);
    printf("test_waterlilies_generation_2 passed\n");
}


void test_waterlilies_generation_3(void) {
    /*
    Tests the case where 2 lanes of waterlilies are following each other
    */

    // We create a custom lane with waterlilies
    lane* waterlilies = (lane*)malloc(sizeof(lane));
    waterlilies->type = WATER;
    waterlilies->y = 0;
    waterlilies->speed = 0;
    waterlilies->coins = NULL;
    waterlilies->prev = NULL;
    waterlilies->next = NULL;
    waterlilies->obst_size = 1;
    waterlilies->obstacles = generate_waterlilies(waterlilies);

    // We create a custom lane of waterlilies
    lane* waterlilies2 = (lane*)malloc(sizeof(lane));
    waterlilies2->type = WATER;
    waterlilies2->y = 1;
    waterlilies2->speed = 0;
    waterlilies2->coins = NULL;
    waterlilies2->prev = waterlilies;
    waterlilies2->next = NULL;
    waterlilies2->obst_size = 1;
    waterlilies2->obstacles = generate_waterlilies(waterlilies2);
    waterlilies->next = waterlilies2;


    bool* waterlilies_array = create_obstacles_array(waterlilies->obstacles);
    bool* waterlilies2_array = create_obstacles_array(waterlilies2->obstacles);
    bool* res = array_and(waterlilies_array, waterlilies2_array);
    assert(array_exist(res));
    free(res);
    free(waterlilies_array);
    free(waterlilies2_array);
    free_lanes(waterlilies);
    printf("test_waterlilies_generation_3 passed\n");
}

void test_waterlilies_generation_4(void) {
    /*
    Tests the case where 2 lanes of waterlilies are separated by a lane of vehicles
    */

    // We create a custom lane with waterlilies
    lane* waterlilies = (lane*)malloc(sizeof(lane));
    waterlilies->type = WATER;
    waterlilies->y = 0;
    waterlilies->speed = 0;
    waterlilies->coins = NULL;
    waterlilies->prev = NULL;
    waterlilies->next = NULL;
    waterlilies->obst_size = 1;
    waterlilies->obstacles = generate_waterlilies(waterlilies);


    // We create a custom lane of vehicles
    lane* vehicles = generate_lane(waterlilies, ROAD);
    waterlilies->next = vehicles;


    // We create a custom lane of waterlilies
    lane* waterlilies2 = (lane*)malloc(sizeof(lane));
    waterlilies2->type = WATER;
    waterlilies2->y = 2;
    waterlilies2->speed = 0;
    waterlilies2->coins = NULL;
    waterlilies2->prev = vehicles;
    waterlilies2->next = NULL;
    waterlilies2->obst_size = 1;
    waterlilies2->obstacles = generate_waterlilies(waterlilies2);
    vehicles->next = waterlilies2;


    bool* waterlilies2_array = create_obstacles_array(waterlilies2->obstacles);
    assert(reachable(vehicles, waterlilies2_array, vehicles->speed));
    free(waterlilies2_array);
    free_lanes(waterlilies);
    printf("test_waterlilies_generation_4 passed\n");
}

void test_waterlilies_generation(void) {
    // Tests the generation of waterlilies in various configurations
    test_waterlilies_generation_1();
    test_waterlilies_generation_2();
    test_waterlilies_generation_3();
    test_waterlilies_generation_4();
    printf("test_waterlilies_generation passed\n");}

int main(void) {
    srand(time(NULL));
    test_update_vehicles();
    test_trees_generation();
    test_update_drowning_slots();
    test_update_trains();
    test_waterlilies_generation();
    return 0;
}
