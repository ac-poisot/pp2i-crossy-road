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
        free(current_lane->coins);
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
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
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


    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = !(rand() % 30); // for each tile, 1/30 chance of having a coin
    }
    new_lane->prev = prev_lane;
    new_lane->next = NULL;
    return new_lane;
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

obstacle* generate_trees(void) {
    /* generates trees */
    /* generates a number of obtacles between 1 and 4 with x and adds it to obstacles*/
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


obstacle* generate_waterlilies(void) {
    /* generates waterlilies */
    /* generates a number of obtacles between 1 and 4 with x and adds it to obstacles*/
    int nb = (int)(3+rand()%3);
    float water_length = (float)(rand()%LANE_WIDTH-2); // taille de l'eau
    float current_x = -1;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = 1;
    obstacle *last_generated = first_obst;
    current_x = current_x + water_length;
    for (int i=0; i<nb-1 && ((int)current_x < LANE_WIDTH-UNPLAYABLE_WIDTH+1) ; i=i+1) {
        water_length = (float)(rand()%(LANE_WIDTH-(int)current_x));
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x+1;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = 1;
        last_generated->next = obst;
        last_generated = obst;
        current_x = current_x + water_length+1;
        if (i==nb-2 && current_x<LANE_WIDTH-UNPLAYABLE_WIDTH+1) {
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
            new_lane->obstacles = generate_trees();
            new_lane->speed = 0;
            break;
        case WATER: // generates waterlilies or drowning slots
            if ((int)(rand()%3) == 0) {
                new_lane->obstacles = generate_waterlilies();
                new_lane->speed = 0;
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
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
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
        } else if ((c->x + l->speed + l->obst_size < 1) && (l->speed < 0)){ // if the vehicle is going out of the lane at the left side, we delete it
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
        } else if ((c->x + l->speed + c->size < 1) && (l->speed < 0)){ // if the slot is going out of the lane at the left side, we delete it
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
        } else if ((c->x + c->size < 1) && (l->speed < 0)){ // if the train is going out of the lane at the left side, we delete it
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
    player p = {PLAYER_START_Y, LANE_WIDTH/2, 0, 0};

    // Initialize the first lane
    lane* l = empty_lane(NULL, GRASS);
    lane* first_lane = l;

    // Generate the first lanes

    for (int i = 0; i < 10; i++) {
         l = empty_lane(l, GRASS);
    }


    // for (int i = 0; i < game_height + 4; i++) {
    //     l = generate_lane(l, WATER);
    // }
    generateNNewLanes(l, game_height + 4);

    displayedData res = {game_height, first_lane, first_lane, p, 0, 0};

    return res;
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
