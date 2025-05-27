
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "core.h"

int tick;

void free_obstacles(obstacle* first_obstacle) {
    /* frees recursively obstacles */
    obstacle* current_obstacle = first_obstacle;
    obstacle* next_obstacle;
    while (current_obstacle != NULL) {
        next_obstacle = current_obstacle->next;
        free(current_obstacle);
        current_obstacle = next_obstacle;
    }
}

void free_lanes(lane* first_lane) {
    /* frees recursively lanes */
    lane* current_lane = first_lane;
    lane* next_lane;
    while (current_lane != NULL) {
        next_lane = current_lane->next;
        float_list* current_coin = current_lane->coins;
        float_list* next_coin;
        while (current_coin != NULL) {
            next_coin = current_coin->next;
            free(current_coin);
            current_coin = next_coin;
        }
        free_obstacles(current_lane->obstacles);
        free(current_lane);
        current_lane = next_lane;
    }
}


int biome(int* boules){// Sa prend les boules de chaque couleur, et renvoit la couleur pioché
    /* chooses randomly a number */
    int nbr_de_boule= boules[1] + boules[2] + boules[3] + boules[0];
    int boule_choisi =rand()%(nbr_de_boule+1);
    //return boule_choisi;
    tick++;
    if (boule_choisi < boules[0]) {
        return 1;
    }
    else if (boule_choisi < boules[1]+boules[0]) {
        return 2;
    }
    else if (boule_choisi<boules[1]+boules[2]+boules[0]) {
        return 3;
    }
    else {
        return 4;
    }
}

int weight(int index) {
    /* weigth for probabilities depending on time */
    return (4-index) * tick;
}

int* probabilite_biomes(lane* l) {// prend les 4 lignes et renvoie les probabilité dans le panier
    /* creates the urn for the generation of biomes */
    int* boules = malloc(4*sizeof(int));
    boules[0] = 1000; // Initializes the urn to give every biome an initial probability to be chosen
    boules[1] = 1000;
    boules[2] = 1000;
    boules[3] = 1000;
    for (int i=0;i<4;i++){ // Makes the probabilities depend on the 4 last generated lanes
        boules[l->type - 1] -= weight(i);
        l = l->next;
    }
    return boules;
}// Cette version ne prend pas en compte l'avancée dans le temps


lane* empty_lane(lane* prev_lane, int type) {
    /* creates an empty lane */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) { // if there is a previous lane, the new lane is created after it
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else { // if there is no previous lane, the new lane is the first one
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    new_lane->speed = 0;
    new_lane->obstacles = NULL;
    new_lane->obst_size = 1+rand()%2;
    new_lane->coins = NULL;
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}


lane* initialLanes(void) {
    /* initializes the four first lanes */
    lane* l = empty_lane(NULL, GRASS);
    lane* firstLane = l;
    for (int i = 0; i < 4; i++) {
        l = empty_lane(l, GRASS);
    }
    return firstLane;
}

lane* random_lane(lane* prev_lane) {
    /* creates a random lane for testing purposes */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane == NULL) {
        new_lane->y = 0;
    } else {
        prev_lane->next = new_lane;
        new_lane->y = prev_lane->y + 1;
    }
    new_lane->speed = 0;

    // Only one obstacle, for testing purposes
    new_lane->obstacles = malloc(sizeof(obstacle));
    new_lane->obstacles->next = NULL;
    new_lane->obstacles->prev = NULL;
    new_lane->obstacles->x = (rand() % (LANE_WIDTH - UNPLAYABLE_WIDTH * 2)) + UNPLAYABLE_WIDTH ;
    new_lane->type = (rand() % 4) + 1;

    if (new_lane->type == WATER) {
        new_lane->obst_size = 5;
    } else {
        new_lane->obst_size = 1;
    }


    new_lane->coins = NULL;
    new_lane->prev = prev_lane;
    new_lane->next = NULL;
    return new_lane;
}

bool* create_obstacles_array(obstacle* o) {
    /* creates an array of booleans representing  */
    bool* array = (bool*)malloc(LANE_WIDTH * sizeof(bool));
    for(int i = 0 ; i < LANE_WIDTH; i++) {
        array[i] = false;
    }
    while(o != NULL) {
        if(o->x >= 0 && o->x < LANE_WIDTH) {
            for(int i = 0; i < o->size; i++) {
                array[(int)round(o->x) + i] = true;
            }
        }
        o = o->next;
    }
    return array;
}

void array_not(bool* a) {
    /* Invert the value of each cell of the array */
    for(int i = 0; i < LANE_WIDTH; i++) {
        a[i] = !a[i];
    }
}

bool* array_and(bool* a1, bool* a2) {
    /* Returns the intersection of two arrays as a new array */
    bool* res = (bool*)malloc(LANE_WIDTH * sizeof(bool));
    for(int i = 0; i < LANE_WIDTH; i++) {
        res[i] = a1[i] && a2[i];
    }
    return res;
}

bool array_exist(bool* a) {
    /* Check if at least one cell of the array is true */
    for(int i = 0; i < LANE_WIDTH; i++) {
        if(a[i]) return true;
    }
    return false;
}

bool reachable(lane* l, bool* a, float speed) { // Check if at least one waterlily is reachable
    if(speed > 0) { // If the cars or trunks are going to the right side
        int i = 5 + UNPLAYABLE_WIDTH;
        if(l->prev != NULL && l->prev->type == WATER && l->prev->speed == 0) {
            // If it is a lane of waterlilies before the road or the lane of trunks, we make sure that at least one waterlily is accessible
            int first = UNPLAYABLE_WIDTH;
            bool* prev_obst = create_obstacles_array(l->prev->obstacles);
            while(first < LANE_WIDTH-UNPLAYABLE_WIDTH && prev_obst[first] != true) {
                first++;
            }
            if(first >= LANE_WIDTH - UNPLAYABLE_WIDTH) {
                return false;
            }
            free(prev_obst);
            i = first;
        }

        for(; i < LANE_WIDTH - UNPLAYABLE_WIDTH; i++) {
            if(a[i]) {
                return true;
            }
        }

    } else if(speed < 0) { // If the cars or trunks are going to the left side
        int upper = LANE_WIDTH-6 - UNPLAYABLE_WIDTH;
        if(l->prev != NULL && l->prev->type == WATER && l->prev->speed == 0) {
            // If it is a lane of waterlilies before the road or the lane of trunks, we make sure that at least one waterlily is accessible
            int last = LANE_WIDTH-6 - UNPLAYABLE_WIDTH;
            bool* prev_obst = create_obstacles_array(l->prev->obstacles);
            while(last >= UNPLAYABLE_WIDTH && prev_obst[last] != true) {
                last--;
            }
            if(last < 0) {
                return false;
            }
            free(prev_obst);
            upper = last;
        }

        for(int i = 0; i <= upper; i++) {
            if(a[i]) {
                return true;
            }
        }
    } else { // If the speed is zero, this test is called between the previous lane is a lane of trees and the one before was one with waterlilies
        bool* reachable_spots = (bool*)malloc(LANE_WIDTH * sizeof(bool));
        bool* tree_obst_array = create_obstacles_array(l->obstacles);
        obstacle* current_tree = l->obstacles;
        while(current_tree != NULL) {
            if(current_tree->x >= 0 && current_tree->x + current_tree->size <= LANE_WIDTH) {
                int i = current_tree->x;
                while(i >= 0 && tree_obst_array[i] != true) {
                    reachable_spots[i] = true; // We can reach this spot
                    i--;
                }
                i = current_tree->x + 1;
                while(i < LANE_WIDTH && tree_obst_array[i] != true) {
                    reachable_spots[i] = true; // We can reach this spot
                    i++;
                }
            }
            current_tree = current_tree->next;
        }
        for(int i =0; i < UNPLAYABLE_WIDTH; i++) {
            reachable_spots[i] = false; // We cannot reach the unplayable spots
        }
        for(int i = LANE_WIDTH - UNPLAYABLE_WIDTH; i < LANE_WIDTH; i++) {
            reachable_spots[i] = false; // We cannot reach the unplayable spots
        }
        bool* res = array_and(reachable_spots, a);
        bool exists = array_exist(res);
        free(reachable_spots);
        free(tree_obst_array);
        free(res);
        return exists;
    }
    return false;
}

obstacle* generate_vehicles(lane* l) {
    /* generates vehicles */
    float x = - (float)(rand()%VEHICLE_INTERVAL*2); // position of the first vehicle
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = l->obst_size;
    obstacle *last_generated = first_obst;
    while (current_x < LANE_WIDTH) { // generates vehicles until the end of the lane
        current_x += (float)(VEHICLE_INTERVAL + VEHICLE_INTERVAL*(rand()%2)); // the distance is calculated randomly between the minimum and maximum distance between two vehicles
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = l->obst_size;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}

obstacle* generate_random_trees(void) {
    /* randomly generates trees */
    int nb = (int)(1+(rand()%3));
    float x = (float)(rand()%(LANE_WIDTH/2));
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = 1;
    obstacle *last_generated = first_obst;
    for (int i=0; i<nb-1 && (LANE_WIDTH-(int)current_x != 0); i=i+1) {
        current_x = current_x + 1 + (float)(rand()%(LANE_WIDTH-(int)current_x));
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = 1;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}

obstacle* generate_trees(lane* l) {
    /* generates trees */
    /* generates a number of obtacles between 1 and 4 with x and adds it to obstacles*/
    if(l->prev == NULL || l->prev->type != WATER || l->prev->speed != 0) {
        return generate_random_trees();
    }
    // If there are waterlilies before, at least one must not be blocked by a tree
    bool* prev_obst = create_obstacles_array(l->prev->obstacles);
    obstacle* current_obst = generate_random_trees();

    bool* current_obst_array = create_obstacles_array(current_obst);
    array_not(current_obst_array);
    bool* intersection = array_and(prev_obst, current_obst_array);
    while(!array_exist(intersection)) { // While there isn't an exit from one of the waterlilies, we regenerate the list of trees
        free(intersection);
        free_obstacles(current_obst);
        free(current_obst_array);
        current_obst = generate_random_trees();
        current_obst_array = create_obstacles_array(current_obst);
        array_not(current_obst_array);
        intersection = array_and(prev_obst, current_obst_array);
    }

    free(intersection);
    free(prev_obst);
    free(current_obst_array);
    return current_obst;
}

obstacle* generate_random_waterlilies(void) {
    /* randomly generates waterlilies */
    int nb = (int)(3+rand()%3); // the number of waterlilies to generate
    float water_length = (float)(rand()%(LANE_WIDTH-2 - 2 * UNPLAYABLE_WIDTH)); // taille de l'eau
    float current_x = -1; // the coordinate of the waterlilies is stored in this variable
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = 1;
    obstacle *last_generated = first_obst;
    current_x = current_x + water_length + UNPLAYABLE_WIDTH;

    for (int i=0; i<nb-1 && ((int)current_x < LANE_WIDTH-UNPLAYABLE_WIDTH-1) ; i=i+1) {
        water_length = (float)(rand()%(LANE_WIDTH-(int)current_x - UNPLAYABLE_WIDTH));
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x+1;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = 1;
        last_generated->next = obst;
        last_generated = obst;
        current_x = current_x + water_length+1;
        if (i==nb-2 && current_x<LANE_WIDTH-UNPLAYABLE_WIDTH-1) {
            // creates a last obstacle to avoid to many waterlilies
            water_length = (float)(LANE_WIDTH-UNPLAYABLE_WIDTH-current_x);
            obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
            obst->x = current_x+1;
            obst->next = NULL;
            obst->prev = last_generated;
            obst->size = 1;
            last_generated->next = obst;
            last_generated = obst;
        }
    }
    return first_obst;
}

obstacle* generate_waterlilies(lane* l) {
    /* generates waterlilies */
    /* generates a number of obtacles between 1 and 4 with x and adds it to obstacles*/
    if(l->prev == NULL) {
        return generate_random_waterlilies();
    }
    obstacle* current_obst = NULL;
    bool* prev_obst = NULL;
    bool* current_obst_array = NULL;
    switch (l->prev->type) {
        case GRASS:
        /* if the previous lane was a grass lane, at least one path should not be blocked by a lack of waterlilies */
            prev_obst = create_obstacles_array(l->prev->obstacles);
            array_not(prev_obst);
            current_obst = generate_random_waterlilies();
            current_obst_array = create_obstacles_array(current_obst);
            bool* intersection = array_and(current_obst_array, prev_obst);
            while(!array_exist(intersection) || (l->prev->prev != NULL && l->prev->prev->type == WATER && l->prev->prev->speed == 0 && !reachable(l->prev, current_obst_array, l->prev->speed))) { // While there isn't an exit to one of the waterlilies, we regenerate the obstacles && if there are waterlilies before, one path must exist between one of the waterlilies of both lanes
                free_obstacles(current_obst);
                free(current_obst_array);
                free(intersection);
                current_obst = generate_random_waterlilies();
                current_obst_array = create_obstacles_array(current_obst);
                intersection = array_and(current_obst_array, prev_obst);
            }
            free(intersection);
            free(current_obst_array);
            free(prev_obst);
            return current_obst;
        case TRACK:
        /* if the previous lane was a track lane, there isn't much constraints */
            return generate_random_waterlilies();
        case WATER:
            if(l->prev->speed == 0) {

                /* if the previous lane was a lane of waterlilies, we generate new waterlilies on the lane such as each waterlilies way is still going */
                prev_obst = create_obstacles_array(l->prev->obstacles);

                current_obst_array = (bool*)malloc(LANE_WIDTH * sizeof(bool));
                for(int i = 0; i < LANE_WIDTH; i++) {
                    current_obst_array[i] = false; // Default value
                }

                int len_group = 0;
                for(int i = 0; i < LANE_WIDTH; i++) {
                    if(prev_obst[i] && len_group <= 2) { // We are currently looking at a group of waterlilies sitting next to each other
                        len_group++;
                    } else if(len_group != 0) { // We just finished a group of waterlilies
                        int chosen = i - len_group + rand()%len_group; // We randomly choose a waterlily in the group to expand it to this new lane
                        current_obst_array[chosen] = true;
                        len_group = 0;


                        // Randomly generates a pattern with one or more waterlilies at that place
                        int pattern = rand()%9;
                        if(pattern == 0) {
                            if(chosen - 2 >= UNPLAYABLE_WIDTH) {
                                current_obst_array[chosen-2] = true;
                            }
                            if(chosen - 1 >= UNPLAYABLE_WIDTH) {
                                current_obst_array[chosen-1] = true;
                            }
                        } else if(pattern == 1 && chosen-1 >= UNPLAYABLE_WIDTH) {
                            current_obst_array[chosen-1] = true;
                        } else if(pattern <= 3) {
                            if(chosen - 1 >= 0) {
                                current_obst_array[chosen-1] = true;
                            }
                            if(chosen + 1 < LANE_WIDTH - UNPLAYABLE_WIDTH) {
                                current_obst_array[chosen+1] = true;
                            }
                        } else if(pattern == 4 && chosen + 1 < LANE_WIDTH - UNPLAYABLE_WIDTH) {
                            current_obst_array[chosen + 1] = true;
                        } else if(pattern == 5) {
                            if(chosen + 1 < LANE_WIDTH - UNPLAYABLE_WIDTH) {
                                current_obst_array[chosen+1] = true;
                            }
                            if(chosen + 2 < LANE_WIDTH - UNPLAYABLE_WIDTH) {
                                current_obst_array[chosen+2] = true;
                            }

                        }
                        
                        if(prev_obst[i]) {
                            len_group++;
                        }
                    }
                }
                if(len_group != 0) { // We just finished a group of waterlilies
                    int chosen = LANE_WIDTH - len_group + rand()%len_group; // We randomly choose a waterlily in the group to expand it to this new lane
                    current_obst_array[chosen] = true;
                    len_group = 0;


                    // Randomly generates a pattern with one or more waterlilies at that place
                    int pattern = rand()%9;
                    if(pattern == 0) {
                        if(chosen - 2 >= 0) {
                            current_obst_array[chosen-2] = true;
                        }
                        if(chosen - 1 >= 0) {
                            current_obst_array[chosen-1] = true;
                        }
                    } else if(pattern <= 3 && chosen - 1 >= 0) {
                        current_obst_array[chosen-1] = true;
                   
                    } else if(pattern == 5) {
                        if(chosen + 1 < LANE_WIDTH) {
                            current_obst_array[chosen+1] = true;
                        }
                        if(chosen + 2 < LANE_WIDTH) {
                            current_obst_array[chosen+2] = true;
                        }

                    }
                }

                // We generate the list of waterlilies from the array of waterlilies
                obstacle* last_generated = NULL;
                for(int i = 0; i < LANE_WIDTH; i++) {
                    if(current_obst_array[i]) {
                        obstacle* new_waterlily = (obstacle*)malloc(sizeof(obstacle));
                        new_waterlily->next = NULL;
                        new_waterlily->prev = last_generated;
                        new_waterlily->size = 1;
                        new_waterlily->x = i;
                        if(last_generated != NULL) {
                            last_generated->next = new_waterlily;
                        }
                        last_generated = new_waterlily;
                        if(current_obst == NULL) {
                            current_obst = new_waterlily;
                        }
                    }
                }

                free(prev_obst);
                free(current_obst_array);
                return current_obst;
            }
            // If there are not waterlilies but trunks, we treat the case as we do for cars hence no "break" instruction here
        case ROAD:
        /* if the previous lane is either a lane of cars or trunks */
        current_obst = generate_random_waterlilies();
        current_obst_array = create_obstacles_array(current_obst);
        int max_regenerate = 0;
        while(!reachable(l->prev, current_obst_array, l->prev->speed)) { // We regenerate the obstacles if the map isn't playable with them because there isn't an accessible way
            free_obstacles(current_obst);
            free(current_obst_array);
            current_obst = generate_random_waterlilies();
            current_obst_array = create_obstacles_array(current_obst);
            max_regenerate++;
            if(max_regenerate == 10 && l->prev->prev != NULL && l->prev->prev->type == WATER && l->prev->prev->speed == 0) {
                l->prev->prev->obstacles = generate_waterlilies(l->prev->prev);
                l->prev->prev->coins = NULL;
                l->prev->speed *= -1;
                max_regenerate = 0;
            }
        }
        free(current_obst_array);
        return current_obst;
    }
}


obstacle* generate_drowning_slots(void) {
    /* generates the slots where the player would drown */
    float x = (float)(rand()%LOG_SPACING_MAX - 8); // position of the first slot
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = 1 + (rand()%MAX_LOG_SIZE); // size of the first slot chosen randomly between 1 and MAX_LOG_SIZE
    obstacle *last_generated = first_obst;
    while(current_x < LANE_WIDTH) {
        current_x += (float)last_generated->size + (float)(rand()%LOG_SPACING_MAX); // the distance between two slots is chosen randomly between 0 and LOG_SPACING_MAX
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = 1 + (rand()%MAX_LOG_SIZE); // size of the slot chosen randomly between 1 and MAX_LOG_SIZE
        last_generated-> next = obst;
        last_generated = obst;
    }
    return first_obst;
}

obstacle* generate_trains(void) {
    float x = -(float)(rand()%TRAIN_LENGTH); // position of the first train
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = TRAIN_LENGTH;
    obstacle *last_generated = first_obst;
    while (current_x < LANE_WIDTH) {
        current_x += (float)(TRAIN_LENGTH + TRAIN_SPACING_MIN + rand()%(TRAIN_SPACING_MAX-TRAIN_SPACING_MIN) + WARNING_TIME*TRAIN_SPEED); // the distance between two trains is chosen randomly between TRAIN_SPACING_MIN and TRAIN_SPACING_MAX
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = TRAIN_LENGTH;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}


float_list* init_coin(float_list* next,float val) {
    float_list* new_coin = malloc(sizeof(float_list));
    new_coin->next = next;
    new_coin->power = 0;
    if (rand()%POWER_PROBABILITY == 0) {
        new_coin->power = rand()%(POWERS_END-1) + 1;
    }
    new_coin->val = val;
    return new_coin;
}

float_list* generate_coins(lane* l) {
    float_list *new_coins = NULL;
    bool* obstacles_array = NULL;
    switch (l->type) {
        case GRASS:
            obstacles_array = create_obstacles_array(l->obstacles);
            int pos = rand()%LANE_WIDTH;
            while(obstacles_array[pos]) {
                pos = rand()%LANE_WIDTH;
            }
            new_coins = (float_list*) init_coin(NULL,(float) pos);
            free(obstacles_array);
            return new_coins;
            break;
        case WATER:
            if(l->speed == 0) {
                obstacles_array = create_obstacles_array(l->obstacles);
                int pos = rand()%LANE_WIDTH;
                while(!obstacles_array[pos]) {
                    pos = rand()%LANE_WIDTH;
                }
                new_coins = (float_list*) init_coin(NULL, pos);
                free(obstacles_array);
                return new_coins;
            } else {
                obstacle *o = l->obstacles;
                float_list *first_coin = NULL;
                float_list *last_coin = NULL;
                while(o != NULL) {
                    if(rand()%COIN_ISSUES == 0 && o->next != NULL) {
                        float_list *new_coins = (float_list*) init_coin(NULL,o->x + rand()%(o->size));
                        if(first_coin == NULL) {
                            first_coin = new_coins;
                        } else {
                            last_coin->next = new_coins;
                        }
                        last_coin = new_coins;
                    }
                    o = o->next;
                }
                return first_coin;
            }
            break;
        case TRACK:
        case ROAD:

            new_coins = (float_list*) init_coin(NULL,rand()%LANE_WIDTH);
            return new_coins;
            break;
        default:
        return NULL;
    }
}


lane* generate_lane(lane* prev_lane, int type) {
    /* creates an empty lane */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) { // if there is a previous lane, the new lane is created after it
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else { // if there is no previous lane, the new lane is the first one
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    new_lane->obst_size = 1+rand()%2;
    switch (type) {
        case GRASS: // generates trees
            new_lane->obstacles = generate_trees(new_lane);
            new_lane->speed = 0;
            break;
        case WATER: // generates waterlilies or drowning slots
            if ((int)(rand()%3) == 0) {
                new_lane->speed = 0;
                new_lane->obstacles = generate_waterlilies(new_lane);
            } else {
                new_lane->obstacles = generate_drowning_slots();
                if (rand()%2) {
                    new_lane->speed = -LOG_SPEED;
                } else {
                    new_lane->speed = LOG_SPEED;
                }
            }
            break;
        case TRACK: // generates trains
            new_lane->obstacles = generate_trains();
            if (rand()%2) {
                new_lane->speed = -TRAIN_SPEED;
            } else {
                new_lane->speed = TRAIN_SPEED;
            }
            break;
        case ROAD: // generates vehicles
            new_lane->obstacles = generate_vehicles(new_lane);
            if (rand()%2) {
                new_lane->speed = VEHICLE_SPEED_MIN + (float)(rand()%2)/10;
            } else {
                new_lane->speed = -VEHICLE_SPEED_MIN - (float)(rand()%2)/10;
            }
            break;
    } // generates coins (not done yet)
    new_lane->type = type;
    if(rand()%COIN_ISSUES == 0) {
        new_lane->coins = generate_coins(new_lane);
    } else {
        new_lane->coins = NULL;
    }

    new_lane->next = NULL;
    
    return new_lane;
}


void update_vehicles(lane* l) {
    /* updates the position of the vehicles and add a new one when the next displayed vehicle doesn't exist */
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed+c->size > VEHICLE_INTERVAL) { // if we can add a vehicle to the left of the first vehicle, we create one
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        float new_x = c->x - (float)(VEHICLE_INTERVAL + VEHICLE_INTERVAL*(rand()%2));
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        new_obst->size = l->obst_size;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) { // updates the position of the vehicles
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){ // if the vehicle is going out of the lane at the right side, we delete it
            c-> prev ->next = NULL;
            if(c->next != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + l->speed + l->obst_size < 0) && (l->speed < 0)){ // if the vehicle is going out of the lane at the left side, we delete it
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) { // if we can add a vehicle to the right of the last vehicle, we create one
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            float current_x = c->x + (float)(VEHICLE_INTERVAL + VEHICLE_INTERVAL*(rand()%2));
            next_obst->x = current_x;
            next_obst->size = l->obst_size;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else { // updates the position of the vehicle
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void update_drowning_slots(lane* l) {
    /* updates the position of the drowning slots*/
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed +c->size> 0) { // if we can add a slot to the left of the first slot, we create one
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        new_obst->size = 1 + (rand()%MAX_LOG_SIZE);
        float new_x = c->x - (float)(new_obst->size + (rand()%LOG_SPACING_MAX));
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
        if(rand()%COIN_ISSUES == 0 && c->next != NULL) {
            float_list *new_coins = (float_list*) init_coin(l->coins,c->x + rand()%(c->size));
            l->coins = new_coins;
        }
    }

    while (c != NULL) { // updates the position of the slots
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){ // if the slot is going out of the lane at the right side, we delete it
            c-> prev ->next = NULL;
            if(c->prev != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + l->speed + c->size < 0) && (l->speed < 0)){ // if the slot is going out of the lane at the left side, we delete it
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) { // if we can add a slot to the right of the last slot, we create one
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            next_obst->size = 1 + (rand()%MAX_LOG_SIZE);
            float current_x = c->x + (float)(next_obst->size + (rand()%LOG_SPACING_MAX));
            if(rand()%COIN_ISSUES == 0) {
                float_list *new_coins = (float_list*) init_coin(l->coins,c->x + rand()%(c->size));
                l->coins = new_coins;
            }
            next_obst->x = current_x;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else { // updates the position of the slot
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }

    float_list *current_coin = l->coins;
    while (current_coin != NULL) {
        current_coin->val = current_coin->val + l->speed;
        current_coin = current_coin ->next;
    }
}

void update_trains(lane* l) {
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x + l->speed + c->size > 0) { // if we can add a train to the left of the first train, we create one
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        new_obst->size = TRAIN_LENGTH;
        float new_x = c->x - (float)(TRAIN_LENGTH + TRAIN_SPACING_MIN + rand()%(TRAIN_SPACING_MAX-TRAIN_SPACING_MIN) + WARNING_TIME*TRAIN_SPEED);
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) { // updates the position of the trains
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){ // if the train is going out of the lane at the right side, we delete it
            c-> prev ->next = NULL;
            if(c->prev != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + c->size < 0) && (l->speed < 0)){ // if the train is going out of the lane at the left side, we delete it
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x < LANE_WIDTH) {
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            next_obst->size = TRAIN_LENGTH;
            float current_x = c->x + (float)(TRAIN_LENGTH + TRAIN_SPACING_MIN + rand()%(TRAIN_SPACING_MAX-TRAIN_SPACING_MIN) + WARNING_TIME*TRAIN_SPEED);
            next_obst->x = current_x;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else { // updates the position of the train
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void display_obstacles(obstacle* o) {
    /* displays the position of the vehicles -- debbug function */
    while (o != NULL) {
        printf("x : %f, size : %d\n", o->x, o->size);
        o = o->next;
    }
}

void displayLanes(lane* l) {
    /* displays the lanes */
    int i = 0;
    while (l != NULL) { // displays the lanes (for each lane do:)
        obstacle* current = NULL;
        if(l->obstacles != NULL) { // if there are obstacles
            current = l->obstacles;
            while(current != NULL && (int)current->x + current->size <= 0) { // if the obstacle is not displayed because too far on the left, we skip it
                current = current->next;
            }
            if(current != NULL && current->x < 0) { // if the obstacle is partially displayed, we display it partially
                for(int j = 0; j < current->x + current->size; j++) {
                    printf("🟥");
                }
                i = current->x + current->size; // we update the position of the cursor because we have already displayed some cells
                current = current->next; // we go to the next obstacle
            }
        }
        for (; i < LANE_WIDTH; i++) { // for each cell not displayed yet:
            if(current != NULL && current->x == (float)i) { // if there is an obstacle to display at the x coordinate, we display it
                for(int j = 0; j < current->size; j++) {
                    if(i >= LANE_WIDTH) {
                        break;
                    }
                    printf("🟥");
                    i++; // we update the position of the cursor accordingly
                }
                i--;
                current = current->next; // we go to the next obstacle
            } else { // else we display the color of the lane
                switch (l->type) {
                    case GRASS:
                        printf("🟩");
                        break;
                    case WATER:
                        printf("🟦");
                        break;
                    case TRACK:
                        printf("⬜️");
                        break;
                    case ROAD:
                        printf("⬛️");
                        break;
                }
            }

        }
        printf("\n");
        l = l->next;
    }
}

lane* fourLastLanes(lane* l) {
    /* gets the four previous lanes */
    lane* lastLane = l;
    for (int i = 0; i < 3; i++) {
        lastLane = lastLane->prev;
    }
    return lastLane;
}

void generateNNewLanes(lane* l, int n) {
    /* generates n lanes based on the four previous lanes */
    for (int i = 0; i < n; i++) {
        int* boules = probabilite_biomes(fourLastLanes(l));
        l = generate_lane(l, biome(boules));
        free(boules);
    }
}

obstacle* collides(lane *current_lane, displayedData game) {
    /* checks if the player collides with an obstacle */
    obstacle* current_obstacle = current_lane->obstacles;
    while (current_obstacle != NULL) {
        if (game.player.x+1 > current_obstacle->x && game.player.x < current_obstacle->x + (current_obstacle->size)) {
            return current_obstacle;
        }
        current_obstacle = current_obstacle->next;
    }
    return NULL;
}

float_list* collides_coin(lane *current_lane, displayedData game) {
    /* checks if the player collides with a coin */
    float_list* current_coin = current_lane->coins;
    while (current_coin != NULL) {
        if (round(game.player.x*10) == round(current_coin->val*10)) { // values truncated to first decimal digit to avoid float precision issues
            current_coin->val = -1; // mark the coin as collected
            return current_coin;
        }
        current_coin = current_coin->next;
    }
    return NULL;
}

displayedData move_camera(displayedData data, float speed) {
    // Move the camera

    if ((int) (data.cameraY+speed) - (int) data.cameraY == 1) {
        data.camera_first_lane = data.camera_first_lane->next;

        // Add a new lane
        lane* current_lane = data.camera_first_lane;
        while (current_lane->next != NULL) {
            current_lane = current_lane->next;
        }

        //current_lane = generate_lane(current_lane, WATER);
        generateNNewLanes(current_lane, 1);
    }

    data.cameraY += speed;
    return data;
}


displayedData init_game(int game_height) {
    // Initialize the game
    player p = {PLAYER_START_Y, LANE_WIDTH/2, 0, 0, 0};

    // Initialize the first lane
    lane* l = empty_lane(NULL, GRASS);
    lane* first_lane = l;

    // Generate the first lanes

    for (int i = 0; i < 10; i++) {
         l = empty_lane(l, GRASS);
    }


    // for (int i = 0; i < game_height + 4; i++) {
    //     if (rand()%2) {
    //         l = generate_lane(l, WATER);
    //     } else {
    //         l = generate_lane(l, GRASS);
    //     }
        
    // }
    generateNNewLanes(l, game_height + 4);

    displayedData res = {game_height, first_lane, first_lane, p, 0, 0};

    return res;
}

displayedData power4 (displayedData data) {
    lane* current_lane = data.camera_first_lane;
    while ((int) current_lane->y != (int) data.player.y) {
        current_lane = current_lane->next;
    }
    if (current_lane->y <= data.player.y){
        current_lane = current_lane->next;
    }
    lane* l1 = empty_lane(current_lane->prev, GRASS);
    lane* l2 = empty_lane(l1, GRASS);
    lane* l3 = empty_lane(l2, GRASS);
    l1->next = l2;
    l2->next = l3;
    l3->next = current_lane->next->next->next;
    current_lane->prev->next = l1;
    current_lane->next->next->next->prev = l3;
    current_lane->prev = NULL;
    current_lane->next->next->next = NULL;
    free_lanes(current_lane);
    return data;
}

#ifdef TEST
int main(void) {
    srand(time(NULL));
    lane* firstLane = initialLanes();
    firstLane->speed = 2;
    //generateNNewLanes(firstLane->next->next->next, 100);
    
    //displayLanes(firstLane);

    obstacle* v1 = generate_vehicles(firstLane);
    firstLane->obstacles = v1;
    firstLane->obst_size = -2;
    display_obstacles(firstLane->obstacles);
    printf("\n");
    for(int i=0; i<5; i++) {
        update_vehicles(firstLane);
        display_obstacles(firstLane->obstacles);
        printf("\n");
    }
    //free_obstacles(v1); // ne pas free si v1 lie a firstLane

    free_lanes(firstLane);
    return 0;
}
#endif
